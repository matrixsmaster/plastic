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
#include "wrldgen.h"


WorldGen::WorldGen()
{
	//TODO: RLCG init
}

WorldGen::~WorldGen()
{
}

void WorldGen::GenerateChunk(PChunk buf)
{
	int x,y,z,t;
	voxel v;
	if (!buf) return;
	//FIXME: for debug only
	for (z = 0; z < CHUNKBOX; z++) {
		for (y = 0; y < CHUNKBOX; y++) {
			for (x = 0; x < CHUNKBOX; x++) {
				if (z < 128) {
//					t = (rand() & 3) + 1;
					t = (((y % 2)? x:(x+1)) % 2) + 1 + (z % 2);
					v = (voxel)t;
				} else v = 0;
				if ((x > 108) && (x < 148) && (y > 108) && (y < 148) && (z > 126) && (z < 135)) v = 4;
				(*buf)[z][y][x] = v;
			}
		}
	}
}
