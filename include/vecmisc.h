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

#ifndef VECMISC_H_
#define VECMISC_H_

#include "vecmath.h"

#define SORTVEC_2DDIST	4
#define SORTVEC_3DDIST	0
#define SORTVEC_BYX		1
#define SORTVEC_BYY		2
#define SORTVEC_BYZ		3

#define SWAPVEC(A,B,Sw) { Sw = A; A = B; B = Sw; }

/*
 * PNPOLY - Point Inclusion in Polygon Test
 * (C) W. Randolph Franklin (WRF)
 */
int pnpoly(int nvert, float *vertx, float *verty, float testx, float testy);

//FIXME: use vector2d instead of 3d

///Test inclusion of 2D point in rectangle described by given UpperLeft and BottomRight.
bool isPntRectIsect2D(vector3d aim, vector3d ul, vector3d br);

///Test inclusion of 2D point in polygon with four vertices.
bool isPntPoly4Isect2D(vector3d aim, vector3d a, vector3d b, vector3d c, vector3d d);

///Returns medium point of N-Gon.
vector3d CenterPoint(vector3d* pts, int n);

///Returns true if a value X lies in gap [a;b].
bool AxSect(const double a, const double b, const double x);

///Set of interpolation functions for Z-buffer operations.
vector3d InterpolatePntByX(vector3d a, vector3d b, double x);
vector3d InterpolatePntByY(vector3d a, vector3d b, double y);
vector3d InterpolatePntByZ(vector3d a, vector3d b, double z);
double InterpolateZQ(vector3d* pts, double x, double y);

///Vectors sorting.
void SortVectors(vector3d* arr, const int n, const int axis);

///Default perspective projection.
void PerspectiveD(vector3d* pnt, const vector3d* fov, const vector2di* mid);
void PerspectiveDInv(vector3d* pnt, const vector3d* fov, const vector2di* mid);

///Experimental reverse projection.
void PerspectiveNInv(vector3d* pnt, const vector3d* fov, const vector2di* mid);

///Sets each vector component to range [0;360).
void RotNormDegF(vector3d* r);
void RotNormDegI(vector3di* r);

///Check for point inclusion in some volume.
bool IsPntInsideCubeI(const vector3di* pnt, const vector3di* center, const int side);

///Bilinear int. V is known points (CW from (0,0)). P is point of interest. Returns P with Z calculated.
vector3d BilinearInterpolation(const vector3d* v, const vector3d* p);

///Helper function to fill rectangle information to pass to other functions (e.g. BilinearInt.).
vector2di RectangleCornerK(const int num);

//FIXME: comment
int FindSubRectDI(vector3di* arr, vector3di* ul, vector3di* br, vector3di* cont, const vector3di* zero, const int w, const int h);
//int FindSubRectFI(vector3di* arr, vector3di* ul, vector3di* br, vector3di* cont);

#endif /* VECMISC_H_ */
