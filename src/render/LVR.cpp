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
#include "support.h"
#include "cube.h"

#ifdef LVRDEBUG
#include "debug.h"
#endif

LVR::LVR(DataPipe* pipe)
{
	pipeptr = pipe;
	render = NULL;
	activebuf = 0;
	rendsize = 0;
	mask = NULL;

	zbuf = NULL;
	pbuf = NULL;

	g_w = g_h = 0;
	rot = GenOMatrix();
	scale = vector3d(DEFSCALE);

	fov.X = DEFFOVX;
	fov.Y = DEFFOVY;
	far = DEFFARPLANE;
	fog = DEFFOGPLANE;
	SetFogColor(vector3di(DEFFOGGRAY));

	skies = (pipe)? (new AtmoSky(pipe)):NULL;
}

LVR::~LVR()
{
	if (skies) delete skies;
	if (render) free(render);
	if (zbuf) free(zbuf);
	if (pbuf) delete[] pbuf;
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

	if (pbuf) delete[] pbuf;

#ifdef LVRDOUBLEBUFFERED
	size_t l;

	//reallocate buffers memory (double-buffered output)
	l = rendsize * sizeof(SGUIPixel) * 2;
	render = (SGUIPixel*)realloc(render,l);
	memset(render,0,l);

	l = rendsize * sizeof(int) * 2;
	zbuf = (int*)realloc(zbuf,l);
	memset(zbuf,0,l);

	pbuf = new vector3di[rendsize*2];

#else
	render = (SGUIPixel*)realloc(render,rendsize*sizeof(SGUIPixel));
	zbuf = (int*)realloc(zbuf,rendsize*sizeof(int));
	pbuf = new vector3di[rendsize];

#endif

	return ((render != NULL) && (zbuf != NULL) && (pbuf != NULL));
}

SGUIPixel* LVR::GetRender()
{
#ifdef LVRDOUBLEBUFFERED
	//return NOT active buffer data
	return (render + ((activebuf)? 0:rendsize));
#else
	return render;
#endif
}

void LVR::SetMask(char* m, int w, int h)
{
	mask = m;
	if ((w != g_w) || (h != g_h)) mask = NULL;
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
	RotNormDegF(&eulerot); //norm it to form [0;360)

	//update skies rotation
	if (skies) skies->SetEulerAngles(eulerot);

	//generate rotation matrices
	rx = GenMtxRotX(eulerot.X * M_PI / 180.f);
	ry = GenMtxRotY(eulerot.Y * M_PI / 180.f);
	rz = GenMtxRotZ(eulerot.Z * M_PI / 180.f);
	//and combine them
	xy = Mtx3Mul(rx,ry);
	rot = Mtx3Mul(xy,rz);

#ifdef LVRDEBUG
	dbg_print("LVR Cam Rot = [%.1f, %.1f, %.1f]",eulerot.X,eulerot.Y,eulerot.Z);
#endif
}

void LVR::SetPosition(const vector3d pos)
{
	offset.X = pos.X;
	offset.Y = pos.Z; //swap Y-Z axes
	offset.Z = pos.Y;

#ifdef LVRDEBUG
	dbg_print("LVR Cam Pos = [%.1f, %.1f, %.1f]",pos.X,pos.Z,pos.Y);
#endif
}

void LVR::SetScale(const double s)
{
	scale = vector3d(s);

#ifdef LVRDEBUG
	dbg_print("LVR Scale = %.4f",s);
#endif
}

void LVR::SetFOV(const vector3d f)
{
	fov = f;

#ifdef LVRDEBUG
	dbg_print("LVR FOV = [%.2f, %.2f]",f.X,f.Y);
#endif
}

void LVR::SetFarDist(const int d)
{
	far = d;
	dfog = 1.f / (double)(far - fog);

#ifdef LVRDEBUG
	dbg_print("LVR Far plane = %d",d);
#endif
}

void LVR::SetFogStart(const int d)
{
	fog = d;
	dfog = 1.f / (double)(far - fog);

#ifdef LVRDEBUG
	dbg_print("LVR Fog dist. = %d",d);
#endif
}

void LVR::SetFogColor(const vector3di nfc)
{
	fogcol = nfc;
	dfog = 1.f / (double)(far - fog);

#ifdef LVRDEBUG
	dbg_print("LVR Fog color: [%d, %d, %d]",nfc.X,nfc.Y,nfc.Z);
#endif
}

vector3di LVR::GetProjection(const vector2di pnt)
{
	vector3di r(-1);

	//check for valid co-ords
	if ((!pbuf) || (pnt.X < 0) || (pnt.Y < 0)) return r;
	if ((pnt.X >= g_w) || (pnt.Y >= g_h)) return r;

#ifdef LVRDOUBLEBUFFERED
	//get a result (NOT active)
	r = pbuf[pnt.Y * g_w + pnt.X + ((activebuf)? 0:rendsize)];
#else
	r = pbuf[pnt.Y * g_w + pnt.X];
#endif

	return r;
}

void LVR::Frame()
{
	int x,y,z,l,fl,s,i,m;
	double fg;
	vector3d v,fo,fn;
	vector3di iv,av;
	SVoxelInf* vox;
	SGUIPixel* frame;
	int* curzbuf;
	vector3di* curpbuf;
	voxel area[6];

	frame = render;
	curzbuf = zbuf;
	curpbuf = pbuf;

#ifdef LVRDOUBLEBUFFERED
	if (activebuf) {
		frame = &render[rendsize];
		curzbuf = &zbuf[rendsize];
		curpbuf = &pbuf[rendsize];
	}
#endif

	memset(curzbuf,0,rendsize*sizeof(int));

	if (skies)
		skies->RenderTo(frame,g_w,g_h);

	/* Scanline renderer */
	for (y = 0, l = 0; y < g_h; y++) {
		for (x = 0; x < g_w; x++,l++) {
			curpbuf[l] = vector3di(-1);
			if ((mask) && (mask[l])) continue;

			//reverse painter's algorithm
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
					curpbuf[l] = iv;
					curzbuf[l] = z;

					//gather area information
					m = 1;
					for (i = 0; i < 6; i++) {
						av = iv;
						s = -1 + ((i & 1) * 2);
						switch (i / 2) {
						case 0: av.X += s; break;
						case 1: av.Y += s; break;
						case 2: av.Z += s; break;
						}
						area[i] = pipeptr->GetVoxel(&av);
						m *= area[i]; //must be zero if at least one voxel isn't occupied
					}

					//check visible side
					s = (m == 0)? GetVCubeMajSide(&eulerot,area):-1;

					if ((s >= 0) && (s < 6)) {
						//draw it!
						frame[l].sym = vox->sides[s];

						//apply voxel' color information
						frame[l].bg = vox->pix.bg;
						frame[l].fg = vox->pix.fg;

						//apply simple fog effect
						//FIXME: maybe put this code somewhere outside? To Postprocess
						fl = z - fog;
						if (fl > 0) {
							fg = dfog * fl;
							fo = tripletovecf(frame[l].bg);
							fo *= (1.f - fg);
							fn.X = fogcol.X;
							fn.Y = fogcol.Y;
							fn.Z = fogcol.Z;
							fn *= fg;
							fo += fn;
							frame[l].bg = vecftotriple(fo);

							fo = tripletovecf(frame[l].fg);
							fo *= (1.f - fg);
							fo += fn;
							frame[l].fg = vecftotriple(fo);
						}
					} else {
						//something went wrong
						frame[l].bg.r = 0;
						frame[l].bg.g = 0;
						frame[l].bg.b = 0;
						frame[l].fg = frame[l].bg;
					}
					break;
				} //voxel frame
			} //by Z
		} //by X
	} //by Y
}

void LVR::Postprocess()
{
	//TODO: move fog here
}

void LVR::SwapBuffers()
{
	activebuf = 1 - activebuf;
}
