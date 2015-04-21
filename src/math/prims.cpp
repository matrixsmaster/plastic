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

#include "prims.h"

void GenPlaneQ(int plane, vector3d cnt, double sz, vector3d* buf)
{
	int i;
	if ((!buf) || (sz <= 0)) return;
	for (i = 0; i < 4; i++) buf[i] = cnt;
	sz /= 2;
	switch (plane) {
	case 0: /* XY */
		buf[0].X -= sz;
		buf[0].Y += sz;
		buf[1].X += sz;
		buf[1].Y += sz;
		buf[2].X += sz;
		buf[2].Y -= sz;
		buf[3].X -= sz;
		buf[3].Y -= sz;
		break;
	case 1: /* YZ */
		buf[0].Y -= sz;
		buf[0].Z += sz;
		buf[1].Y += sz;
		buf[1].Z += sz;
		buf[2].Y += sz;
		buf[2].Z -= sz;
		buf[3].Y -= sz;
		buf[3].Z -= sz;
		break;
	case 2: /* ZX */
		buf[0].Z -= sz;
		buf[0].X += sz;
		buf[1].Z += sz;
		buf[1].X += sz;
		buf[2].Z += sz;
		buf[2].X -= sz;
		buf[3].Z -= sz;
		buf[3].X -= sz;
		break;
	}
}

/*
 * 			XY (2)
 * 	ZX		[ 0 ]
 * 	YZ	  1 | 2 | 3		YZ
 * 	ZX	    | 4 |
 * 	XY	    | 5 |
 */
void GenCubeQ(vector3d cnt, double sz, vector3d* buf)
{
	vector3d shf;
	double h = sz / 2;
	if ((!buf) || (sz <= 0)) return;
	shf.X = 0; shf.Y = -h; shf.Z = 0;
	GenPlaneQ(2,(cnt+shf),sz,buf+0); //ZX0
	shf.X = -h; shf.Y = 0; shf.Z = 0;
	GenPlaneQ(1,(cnt+shf),sz,buf+4); //YZ1
	shf.X = 0; shf.Y = 0; shf.Z = h;
	GenPlaneQ(0,(cnt+shf),sz,buf+8); //XY2
	shf.X = h; shf.Y = 0; shf.Z = 0;
	GenPlaneQ(1,(cnt+shf),sz,buf+12); //YZ3
	shf.X = 0; shf.Y = h; shf.Z = 0;
	GenPlaneQ(2,(cnt+shf),sz,buf+16); //ZX4
	shf.X = 0; shf.Y = 0; shf.Z = -h;
	GenPlaneQ(0,(cnt+shf),sz,buf+20); //XY5
}
