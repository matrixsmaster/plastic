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
	int sx,sy,ex,ey,x,y,l;
	vector3di p;
	voxel v;
	SVoxelTab* tab = pipe->GetVoxTable();

	memset(rad,0,g_l*sizeof(SGUIPixel));

	sx = -g_w / 2;
	ex = sx + g_w;
	sy = -g_h / 2;
	ey = sy + g_h;
	p.Y = (sy + center.Y);

	//TODO: create and rotate view frustrum (pyramid)

	for (l = 0, y = sy; y < ey; y++, p.Y+=VOXGRAIN) {
		p.X = (sx + center.X);// * VOXGRAIN;
		for (x = sx; x < ex; x++, p.X+=VOXGRAIN, l++) {

			//check for player's point
			if ((x == 0) && (y == 0)) {
				rad[l].fg.r = 1000;
				rad[l].fg.g = 1000;
				rad[l].fg.b = 1000;
				rad[l].sym = 'X';
				continue;
			}

			//get 3d point
			p.Z = pipe->GetElevationUnder(&p);
			if (p.Z < 0) {
				rad[l].sym = ' ';
				continue;
			}

			//get voxel at this point
			v = pipe->GetVoxel(&p);
			if ((!v) || (v >= tab->len)) {
				rad[l].sym = ' ';
				continue;
			}

			//select representation
			rad[l].fg = tab->tab[v].pix.fg;
			rad[l].sym = radar_tiles[tab->tab[v].type];

			//check 3D intersection with view frustrum
			//TODO
		}
	}
}
