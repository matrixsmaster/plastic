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

#include "cube.h"

int GetVCubeMajSide(/*const int scrx, const int scry, */const vector3d* rot, const voxel* area)
{
	int i,s,r = 0;
	char mask[6] = {1,1,1,1,1,1};

	//Inclination
	if 		((rot->X > 315) || (rot->X <=  45)) { mask[3] = 0; mask[2]++; }
	else if ((rot->X >  45) && (rot->X <= 135)) { mask[0] = 0; mask[1]++; }
	else if ((rot->X > 135) && (rot->X <= 225)) { mask[2] = 0; mask[3]++; }
	else										{ mask[1] = 0; mask[0]++; }

	//Azimuth
	if 		((rot->Y > 315) || (rot->Y <=  45)) { mask[3] = 0; mask[2]++; }
	else if ((rot->Y >  45) && (rot->Y <= 135)) { mask[4] = 0; mask[5]++; }
	else if ((rot->Y > 135) && (rot->Y <= 225)) { mask[2] = 0; mask[3]++; }
	else										{ mask[5] = 0; mask[4]++; }

	//Area
	for (i = 0, s = 0; i < 6; i++) {
		if (area[i]) mask[i] = 0;
		else if (mask[i] > 1) {
			r = i;
			s = 1;
		} else if (s == 0)
			r = i;
	}

	return r;
}
