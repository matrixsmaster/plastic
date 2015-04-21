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
	table = NULL;
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
	rendsize = w * h;

	render = (SGUIPixel*)realloc(render,rendsize);
	zbuf = (float*)realloc(zbuf,rendsize);

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
	if (!table) return;
}
