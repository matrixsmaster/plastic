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

#ifndef MTX3D_H_
#define MTX3D_H_

#include "vecmath.h"


struct SMatrix3d {
	double m[4][4];
	int r;
};

SMatrix3d GenOMatrix();
SMatrix3d GenMtxRotX(const double fi);
SMatrix3d GenMtxRotY(const double fi);
SMatrix3d GenMtxRotZ(const double fi);

SMatrix3d Mtx3Mul(SMatrix3d A, SMatrix3d B);
vector3d MtxPntMul(SMatrix3d M, vector3d P);

#endif /* MTX3D_H_ */
