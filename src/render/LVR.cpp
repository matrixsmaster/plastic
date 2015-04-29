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

LVR::LVR(DataPipe* pipe)
{
	int i;
	pipeptr = pipe;
	render = NULL;
	zbuf = NULL;
	pbuf = NULL;
	rendsize = 0;
	g_w = g_h = 0;
	far = DEFFARPLANE;
	fov.X = DEFFOVX;
	fov.Y = DEFFOVY;
	scale = vector3d(1);
	skies = (pipe)? (new AtmoSky(DEFSKYLEN,pipe)):NULL;
	for (i = 0; i < 3; i++) rot[i] = GenOMatrix();
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

	//reallocate buffers memory
	render = (SGUIPixel*)realloc(render,rendsize*sizeof(SGUIPixel));
	zbuf = (float*)realloc(zbuf,rendsize*sizeof(float));
	if (pbuf) delete[] pbuf;
	pbuf = new vector3di[rendsize];

	return ((render != NULL) && (zbuf != NULL) && (pbuf != NULL));
}

void LVR::SetEulerRotation(const vector3d r)
{
	rot[0] = GenMtxRotX(r.X * M_PI / 180.f);
	rot[1] = GenMtxRotY(r.Z * M_PI / 180.f); //swap Y-Z axes
	rot[2] = GenMtxRotZ(r.Y * M_PI / 180.f);
	skies->SetEulerAngles(r);
}

void LVR::SetPosition(const vector3d pos)
{
	offset.X = pos.X;
	offset.Y = pos.Z; //swap Y-Z axes
	offset.Z = pos.Y;
	dbg_print("LVR Cam Pos = [%.1f %.1f %.1f]",pos.X,pos.Z,pos.Y);
}

void LVR::SetScale(const double s)
{
	scale = vector3d(s);
	dbg_print("LVR Scale = %.4f",s);
}

void LVR::SetFOV(const vector3d f)
{
	fov = f;
	dbg_print("LVR FOV = [%.2f %.2f]",f.X,f.Y);
}

void LVR::SetFarDist(const int d)
{
	far = d;
	dbg_print("LVR Far plane = %d",d);
}

vector3di LVR::GetProjection(const vector2di pnt)
{
	vector3di r(-1);
	if ((!pbuf) || (pnt.X < 0) || (pnt.Y < 0)) return r;
	if ((pnt.X >= CHUNKBOX) || (pnt.Y >= CHUNKBOX)) return r;
	r = pbuf[pnt.Y * g_w + pnt.X];
	return r;
}

void LVR::Frame()
{
	int x,y,z,l,i;
	vector3d v;
	vector3di iv;
	SVoxelInf* vox;

	memset(zbuf,0,rendsize*sizeof(float));

	skies->RenderTo(render,rendsize);

#if 0
	//current tested: FOV(28,14), scale 0.33
	/* Scanline renderer */
	for (y = 0, l = 0; y < g_h; y++) {
		for (x = 0; x < g_w; x++,l++) {
			//reverse painter's algorithm (+ z-buffer)?
			for (z = 1; z <= far; z++) {
				//make current point vector
				v.X = (double)x;
				v.Y = (double)y;
				v.Z = (double)z;
				//calculate reverse projection into screen space
				PerspectiveDInv(&v,&fov,&mid);
//				v.Y = -v.Y + mid.Y;
//				v.X += mid.X;

				//apply transformations
				//FIXME: use matrix product, not single matrices for each transform
				v *= scale; //actually scales a camera frustrum, not a world
//				for (i = 2; i >= 0; i--)
//					v = MtxPntMul(&rot[i],&v);
				SMatrix3d ft = Mtx3Mul(rot[0],rot[1]);
//				ft = Mtx3Mul(ft,rot[0]);
				v = MtxPntMul(&ft,&v);
//				v = MtxPntMul(&rot[1],&v);

				v += offset;

				//round vector to use as a voxel space co-ord
				iv.X = (int)(round(v.X));// * scale.X);
				iv.Y = (int)(round(v.Z));// * scale.Y); //swap Y-Z axes
				iv.Z = (int)(round(v.Y));// * scale.Z);
				vox = pipeptr->GetVoxelI(&iv);

				//if voxel place is occupied, break current z-axis loop
//				if ((vox->type != VOXT_EMPTY) && ((zbuf[l] > v.Z) || (zbuf[l] == 0))) {
//					zbuf[l] = v.Z;
				if (vox->type != VOXT_EMPTY) {
					pbuf[l] = iv;
					render[l].bg = vox->pix.bg;
					render[l].fg = vox->pix.fg;
					render[l].sym = vox->sides[0]; //FIXME
					break;
				}
			} //by Z
		} //by X
	} //by Y
#elif 0
	/* S-Cube */
	vector3d tmp;
	int j;
	vox = pipeptr->GetVInfo(1);
	for (z = 1; z <= far; z++) {
		/*
		 *    4
		 *  2 0 3
		 *    1
		 */
		for (i = 0; i < 5; i++) {
			for (x = -z; x <= z; x++) {
				for (y = -z; y <= z; y++) {
					switch (i) {
					case 0:
						v.X = x;
						v.Y = y;
						v.Z = z;
						break;
					case 1:
						v.X = x;
						v.Y = -z;
						v.Z = y;
						break;
					case 2:
						v.X = -z;
						v.Y = x;
						v.Z = y;
						break;
					case 3:
						v.X = z;
						v.Y = x;
						v.Z = y;
						break;
					case 4:
						v.X = x;
						v.Y = z;
						v.Z = y;
						break;
					}
					if (v.Z < 1) continue;

					tmp = v;
					for (j = 0; j < 3; j++)
						v = MtxPntMul(&rot[j],&v);
					v += offset;

					iv.X = (int)(round(v.X));
					iv.Y = (int)(round(v.Z)); //swap Y-Z axes
					iv.Z = (int)(round(v.Y));
					vox = pipeptr->GetVoxelI(&iv);
					if (vox->type == VOXT_EMPTY) continue;

//					v -= offset;
//					v.Z = z;
					v = tmp;
					PerspectiveD(&v,&fov,&mid);
//					v.Y = -v.Y + mid.Y;
//					v.X += mid.X;

					l = (int)v.Y * g_w + (int)v.X;
					if ( 	(v.X >= 0) && (v.X < g_w) &&
							(v.Y >= 0) && (v.Y < g_h) &&
							(zbuf[l] == 0) ) {
						pbuf[l] = iv;
						zbuf[l] = v.Z;
						render[l].bg = vox->pix.bg;
						render[l].fg = vox->pix.fg;
						render[l].sym = i + '0';//vox->sides[0]; //FIXME
					}
				} // by Y
			} //by X
		} //by sides
	} //by Z
#else
	/*Brute force cube direct raster */
	for (x = -far; x <= far; x++) {
//		if ((x < 0) || (x >= CHUNKBOX)) continue;
		for (y = -far; y <= far; y++) {
//			if ((y < 0) || (y >= CHUNKBOX)) continue;
			for (z = -far; z <= far; z++) {
//				if ((z < 0) || (z >= CHUNKBOX)) continue;

				iv.X = x + (int)(offset.X);
				iv.Y = z + (int)(offset.Y); //swap!
				iv.Z = y + (int)(offset.Z);

				vox = pipeptr->GetVoxelI(&iv);
				if (vox->type == VOXT_EMPTY) continue;

				v.X = x;
				v.Y = y;
				v.Z = z;
				for (i = 0; i < 3; i++)
					v = MtxPntMul(&rot[i],&v);
//				v += offset;

				if (v.Z < 1) continue;
				PerspectiveD(&v,&fov,&mid);

				l = (int)v.Y * g_w + (int)v.X;
				if ( 	(v.X >= 0) && (v.X < g_w) &&
						(v.Y >= 0) && (v.Y < g_h) &&
						((zbuf[l] == 0) || (zbuf[l] > v.Z))) {
					pbuf[l] = iv;
					zbuf[l] = v.Z;
					render[l].bg = vox->pix.bg;
					render[l].fg = vox->pix.fg;
					render[l].sym = vox->sides[0];
				}
			}
		}
	}
#endif
}
