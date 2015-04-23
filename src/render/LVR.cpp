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

LVR::LVR(DataPipe* pipe)
{
	int i;
	pipeptr = pipe;
	render = NULL;
	zbuf = NULL;
	rendsize = 0;
	g_w = g_h = 0;
	far = DEFFARPLANE;
	fov.X = DEFFOVX;
	fov.Y = DEFFOVY;
	table = pipe->GetVoxTable();
	for (i = 0; i < 3; i++) rot[i] = GenOMatrix();
}

LVR::~LVR()
{
	if (render) free(render);
	if (zbuf) free(zbuf);
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

	return ((render != NULL) && (zbuf != NULL));
}

void LVR::SetEulerRotation(vector3d r)
{
	rot[0] = GenMtxRotX(r.X * M_PI / 180.f);
	rot[1] = GenMtxRotY(r.Y * M_PI / 180.f);
	rot[2] = GenMtxRotZ(r.Z * M_PI / 180.f);
}

void LVR::SetPosition(vector3d pos)
{
	offset = pos;
}

void LVR::Frame()
{
	int x,y,z,l,i;
	vector3d v;
	vector3di iv;
	SVoxelInf* vox;

//	memset(render,0,rendsize*sizeof(SGUIPixel));
	memset(zbuf,0,rendsize*sizeof(float));

	if (!table) return;

	/* Scanline renderer */
	for (y = 0, l = 0; y < g_h; y++) {
		for (x = 0; x < g_w; x++,l++) {
			render[l].col = 1;
			render[l].sym = ' ';
			//reverse painter's algorithm + z-buffer
			for (z = 1; z <= far; z++) {
				//make current point vector
				v.X = (double)x;
				v.Y = (double)y;
				v.Z = (double)z;
				//calculate reverse projection into screen space
				PerspectiveDInv(&v,&fov,&mid);

				//apply transformations
				for (i = 0; i < 3; i++)
					v = MtxPntMul(&rot[i],&v);
				v += offset;

				//round vector to use a voxel co-ord
				iv.X = (int)(round(v.X));
				iv.Y = (int)(round(v.Y));
				iv.Z = (int)(round(v.Z));
				vox = pipeptr->GetVoxelI(&iv);

				//if voxel place is occupied, break current z-axis loop
				if (vox->type != VOXT_EMPTY) {
					render[l].col = vox->color;
					render[l].sym = vox->sides[0]; //FIXME
					break;
				}
			} //by Z
		} //by X
	} //by Y
}
