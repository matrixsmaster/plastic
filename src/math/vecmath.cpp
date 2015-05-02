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

#include "vecmath.h"

CPoint3D CPoint3D::HalfWay(const CPoint3D s)
{
	CPoint3D r;
	r.X = (fabs(X-s.X)/2) + fmin(X,s.X);
	r.Y = (fabs(Y-s.Y)/2) + fmin(Y,s.Y);
	r.Z = (fabs(Z-s.Z)/2) + fmin(Z,s.Z);
	return r;
}

double CPoint3D::DistanceTo(const CPoint3D s)
{
	double r = sqrt((s.X-X)*(s.X-X) + (s.Y-Y)*(s.Y-Y) + (s.Z-Z)*(s.Z-Z));
	return r;
}

int CPoint3D::fcompare(const double a, const double b)
{
	//TODO: urgently needed!
	return 0;
}
