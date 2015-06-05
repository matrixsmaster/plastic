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
#include <string.h>
#include "radar.h"

PlasticRadar::PlasticRadar(DataPipe* pipeptr)
{
	pipe = pipeptr;
	rad = NULL;
	g_w = g_h = g_l = 0;
	center = vector3di(CHUNKBOX/2,CHUNKBOX/2,0);
}

PlasticRadar::~PlasticRadar()
{
	if (rad) free(rad);
}

void PlasticRadar::Resize()
{
	g_l = g_w * g_h;
	rad = (SGUIPixel*)realloc(rad,g_l*sizeof(SGUIPixel));
	memset(rad,0,g_l*sizeof(SGUIPixel));
}

void PlasticRadar::SetWH(int w, int h)
{
	g_w = (w < 0)? 0:w;
	g_h = (h < 0)? 0:h;
	g_l = 0;
	Resize();
}

void PlasticRadar::Update()
{
	int x,y,l;

	for (l = 0, y = (-g_h/2); y < g_h/2; y++)
		for (x = (-g_w/2); x < g_w/2; x++, l++) {
			rad[l].sym = '1';
			rad[l].fg.g = 1000;
		}
}
