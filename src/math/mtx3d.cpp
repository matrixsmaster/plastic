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

#include "mtx3d.h"

SMatrix3d Mtx3Mul(SMatrix3d A, SMatrix3d B)
{
	SMatrix3d res;
	res.r = 0;
	int i,j,k;
	double _s;
	for (i=0; i<4; i++)
		for (j=0; j<4; j++) {
			_s = 0.0;
			for (k=0; k<4; k++)
				_s += A.m[i][k] * B.m[k][j];
			res.m[i][j] = _s;

		}
	return res;
}

vector3d MtxPntMul(const SMatrix3d* M, const vector3d* P)
{
	vector3d res;
	double rk;
	for (int i=0; i<4; i++) {
		rk = P->X*M->m[0][i] + P->Y*M->m[1][i] + P->Z*M->m[2][i] + M->m[3][i];
		switch (i) {
		case 0: res.X = rk; break;
		case 1: res.Y = rk; break;
		case 2: res.Z = rk; break;
		}
	}
	return res;
}

SMatrix3d GenOMatrix()
{
	SMatrix3d res;
	res.r = 0;
	int i,j;
	for (i=0; i<4; i++)
		for (j=0; j<4; j++)
			res.m[i][j] = (i==j)? 1.0:0.0;
	return res;
}

SMatrix3d GenMtxRotX(double fi)
{
	SMatrix3d res = GenOMatrix();
	double sn = sin(fi);
	double cs = cos(fi);
	res.m[1][1] = cs;
	res.m[1][2] = sn;
	res.m[2][1] = -sn;
	res.m[2][2] = cs;
	return res;
}

SMatrix3d GenMtxRotY(double fi)
{
	SMatrix3d res = GenOMatrix();
	double sn = sin(fi);
	double cs = cos(fi);
	res.m[0][0] = cs;
	res.m[0][2] = sn;
	res.m[2][0] = -sn;
	res.m[2][2] = cs;
	return res;
}

SMatrix3d GenMtxRotZ(double fi)
{
	SMatrix3d res = GenOMatrix();
	double sn = sin(fi);
	double cs = cos(fi);
	res.m[0][0] = cs;
	res.m[0][1] = sn;
	res.m[1][0] = -sn;
	res.m[1][1] = cs;
	return res;
}
