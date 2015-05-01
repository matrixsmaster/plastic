/**
 *  Plastic Inquisitor
 *  Copyright (C) 2015 The Plastic Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <stdlib.h>
#include <math.h>
#include "LVR.h"
#include "debug.h"
#include "support.h"


LVR::LVR(DataPipe* pipe)
{
	pipeptr = pipe;
	render = NULL;
	zbuf = NULL;
	pbuf = NULL;
	rendsize = 0;
	g_w = g_h = 0;
	far = DEFFARPLANE;
	fog = DEFFOGPLANE;
	fov.X = DEFFOVX;
	fov.Y = DEFFOVY;
	scale = vector3d(1);
	skies = (pipe)? (new AtmoSky(pipe)):NULL;
	rot = GenOMatrix();
//	fogtab = NULL;
}

LVR::~LVR()
{
	if (skies) delete skies;
	if (render) free(render);
	if (zbuf) free(zbuf);
	if (pbuf) delete[] pbuf;
//	if (fogtab) delete[] fogtab;
}

bool LVR::Resize(int w, int h)
{
	if (w < 1) w = 0;
	if (h < 1) h = 0;

	//apply setting
	g_w = w;
	g_h = h;
	rendsize = w * h;

	//get a screen middle point
	mid.X = w / 2;
	mid.Y = h / 2;

	//reallocate buffers memory
	render = (SGUIPixel*)realloc(render,rendsize*sizeof(SGUIPixel));
	zbuf = (float*)realloc(zbuf,rendsize*sizeof(float));
	if (pbuf) delete[] pbuf;
	pbuf = new vector3di[rendsize];

	return ((render != NULL) && (zbuf != NULL) && (pbuf != NULL));
}

void LVR::RemoveSkies()
{
	if (skies) delete skies;
	skies = NULL;
}

void LVR::SetEulerRotation(const vector3d r)
{
	SMatrix3d rx,ry,rz,xy;

	//store rotation in ordinary form
	eulerot.X = r.X;
	eulerot.Y = r.Z; //swap Y-Z axes
	eulerot.Z = r.Y;

	//update skies rotation
	if (skies) skies->SetEulerAngles(eulerot);

	//generate rotation matrices
	rx = GenMtxRotX(r.X * M_PI / 180.f);
	ry = GenMtxRotY(r.Z * M_PI / 180.f); //swap Y-Z axes
	rz = GenMtxRotZ(r.Y * M_PI / 180.f);
	//and combine them
	xy = Mtx3Mul(rx,ry);
	rot = Mtx3Mul(xy,rz);

	dbg_print("LVR Cam Rot = [%.1f, %.1f, %.1f]",r.X,r.Z,r.Y);
}

void LVR::SetPosition(const vector3d pos)
{
	offset.X = pos.X;
	offset.Y = pos.Z; //swap Y-Z axes
	offset.Z = pos.Y;
	dbg_print("LVR Cam Pos = [%.1f, %.1f, %.1f]",pos.X,pos.Z,pos.Y);
}

void LVR::SetScale(const double s)
{
	scale = vector3d(s);
	dbg_print("LVR Scale = %.4f",s);
}

void LVR::SetFOV(const vector3d f)
{
	fov = f;
	dbg_print("LVR FOV = [%.2f, %.2f]",f.X,f.Y);
}

void LVR::SetFarDist(const int d)
{
	far = d;
	dfog = 1.f / (double)(far - fog);
//	UpdateFogTab();
	dbg_print("LVR Far plane = %d",d);
}

void LVR::SetFogStart(const int d)
{
	fog = d;
	dfog = 1.f / (double)(far - fog);
//	UpdateFogTab();
	dbg_print("LVR Fog dist. = %d",d);
}

void LVR::SetFogColor(const vector3di nfc)
{
	fogcol = nfc;
	dfog = 1.f / (double)(far - fog);
//	UpdateFogTab();
	dbg_print("LVR Fog color: [%d, %d, %d]",nfc.X,nfc.Y,nfc.Z);
}

//void LVR::UpdateFogTab()
//{
//	int i;
//	int l = far - fog + 1;
//	vector3d d,c;
//
//	//remove old tab
//	if (fogtab) delete[] fogtab;
//	fogtab = NULL;
//
//	//check for emptiness
//	if (l < 1) return;
//
//	//create new
//	fogtab = new vector3d[l];
//	if (!fogtab) return;
//
//	//calculate d{R/G/B}
//	d = fogcol / (double)l;
//	c = d; //initial color
//
//	//fill in
//	for (i = 0; i < l; i++) {
//		fogtab[i]
//	}
//}

vector3di LVR::GetProjection(const vector2di pnt)
{
	vector3di r(-1);

	//check for valid co-ords
	if ((!pbuf) || (pnt.X < 0) || (pnt.Y < 0)) return r;
	if ((pnt.X >= CHUNKBOX) || (pnt.Y >= CHUNKBOX)) return r;

	//get a result
	r = pbuf[pnt.Y * g_w + pnt.X];
	return r;
}

void LVR::Frame()
{
	int x,y,z,l,fl;
	double fg;
	vector3d v,fo,fn;
	vector3di iv;
	SVoxelInf* vox;

//	memset(zbuf,0,rendsize*sizeof(float));

	if (skies)
		skies->RenderTo(render,g_w,g_h);

	/* Scanline renderer */
	for (y = 0, l = 0; y < g_h; y++) {
		for (x = 0; x < g_w; x++,l++) {
			pbuf[l] = vector3di(-1);
			//reverse painter's algorithm (+ z-buffer)?
			for (z = 1; z <= far; z++) {
				//make current point vector
				v.X = (double)x;
				v.Y = (double)y;
				v.Z = (double)z;
				//calculate reverse projection from screen into worldspace
				PerspectiveDInv(&v,&fov,&mid);

				//apply transformations
				v *= scale; //actually scales a camera frustrum, not the world
				v = MtxPntMul(&rot,&v);
				v += offset;

				//round vector to use as a voxel space co-ord
				iv.X = (int)(round(v.X));
				iv.Y = (int)(round(v.Z)); //swap Y-Z axes
				iv.Z = (int)(round(v.Y));
				vox = pipeptr->GetVoxelI(&iv);

				//if voxel found isn't empty, draw it and break current z-axis loop
				if (vox->type != VOXT_EMPTY) {
					//remember co-ords for screen raycast
					pbuf[l] = iv;

					//check visible side
					render[l].sym = vox->sides[0]; //FIXME

					//apply voxel' color information
					render[l].bg = vox->pix.bg;
					render[l].fg = vox->pix.fg;

					//apply simple fog effect
					fl = z - fog;
					if (fl > 0) {
						fg = dfog * fl;
						fo = tripletovecf(render[l].bg);
						fo *= (1.f - fg);
						fn.X = fogcol.X;
						fn.Y = fogcol.Y;
						fn.Z = fogcol.Z;
						fn *= fg;
						fo += fn;
						render[l].bg = vecftotriple(fo);

						fo = tripletovecf(render[l].fg);
						fo *= (1.f - fg);
						fo += fn;
						render[l].fg = vecftotriple(fo);
					}

					break;
				} //voxel render
			} //by Z
		} //by X
	} //by Y
}
