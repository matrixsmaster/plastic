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
#include "vecmisc.h"

bool isPntRectIsect(vector3d aim, vector3d ul, vector3d br)
{
	if ((aim.X < ul.X) || (aim.X > br.X)) return false;
	if ((aim.Y < br.Y) || (aim.Y > ul.Y)) return false;
	return true;
}

bool isPntPoly4Isect(vector3d aim, vector3d a, vector3d b, vector3d c, vector3d d)
{
	float x[4],y[4];
	x[0] = a.X; y[0] = a.Y;
	x[1] = b.X; y[1] = b.Y;
	x[2] = c.X; y[2] = c.Y;
	x[3] = d.X; y[3] = d.Y;
	return (pnpoly(4,x,y,aim.X,aim.Y));
}


vector3d InterpolatePntByX(vector3d a, vector3d b, double x)
{
	vector3d d,r;
	if (a.DistanceTo(b) <= 0) return a;
	if (b.X < a.X) SWAPVEC(a,b,d);
	d = b - a;
	if (d.X <= 0) return a;
	r.X = x;
	r.Y = d.Y * (x - a.X) / d.X + a.Y;
	r.Z = d.Z * (x - a.X) / d.X + a.Z;
	return r;
}

vector3d InterpolatePntByY(vector3d a, vector3d b, double y)
{
	vector3d d,r;
	if (a.DistanceTo(b) <= 0) return a;
	if (b.Y < a.Y) SWAPVEC(a,b,d);
	d = b - a;
	if (d.Y <= 0) return a;
	r.X = d.X * (y - a.Y) / d.Y + a.X;
	r.Y = y;
	r.Z = d.Z * (y - a.Y) / d.Y + a.Z;
	return r;
}

vector3d InterpolatePntByZ(vector3d a, vector3d b, double z)
{
	vector3d d,r;
	if (a.DistanceTo(b) <= 0) return a;
	if (b.Z < a.Z) SWAPVEC(a,b,d);
	d = b - a;
	if (d.Z <= 0) return a;
	r.X = d.X * (z - a.Z) / d.Z + a.X;
	r.Y = d.Y * (z - a.Z) / d.Z + a.Y;
	r.Z = z;
	return r;
}

bool AxSect(const double a, const double b, const double x)
{
	double bot = fmin(a,b);
	double top = bot + fabs(a - b);
	return ((bot <= x) && (x <= top));
}

double InterpolateZQ(vector3d* pts, double x, double y)
{
	int i,j;
	vector3d r,l,c,*buf;
	if (!pts) return -1;
	buf = (vector3d*)malloc(sizeof(vector3d)*4);
	if (!buf) return -1;
	/* Find two lines intersecting YZ-plane of interest */
	for (i = 0, j = 0; (i < 4) && (j < 4); i++) {
		l = pts[i];
		r = pts[(i<3)? (i+1):0];
		if (AxSect(l.Y,r.Y,y)) {
			buf[j++] = l;
			buf[j++] = r;
		}
	}
	if (j == 4) {
		/* Calculate basic points on that lines */
		l = InterpolatePntByY(buf[0],buf[1],y);
		r = InterpolatePntByY(buf[2],buf[3],y);
		if ((AxSect(l.X,r.X,x)) && (!AxSect(l.Z,r.Z,0)))
			/* if basic line intersects XZ-plane of interest and not Z0 */
			c = InterpolatePntByX(l,r,x); /* calculate the actual intersection */
		else
			c.Z = -1;
	} else
		c.Z = -1; //out of view
	free(buf);
	return (c.Z);
}

static int compar_x(const void* a, const void* b)
{
	vector3d* ma = (vector3d*)a;
	vector3d* mb = (vector3d*)b;
	return ((int)round(ma->X - mb->X));
}

static int compar_y(const void* a, const void* b)
{
	vector3d* ma = (vector3d*)a;
	vector3d* mb = (vector3d*)b;
	return ((int)round(ma->Y - mb->Y));
}

static int compar_z(const void* a, const void* b)
{
	vector3d* ma = (vector3d*)a;
	vector3d* mb = (vector3d*)b;
	return ((int)round(ma->Z - mb->Z));
}

static int compar_xyz(const void* a, const void* b)
{
	vector3d z;
	vector3d* ma = (vector3d*)a;
	vector3d* mb = (vector3d*)b;
	double aa = z.DistanceTo(*ma);
	double bb = z.DistanceTo(*mb);
	return ((int)round(aa - bb));
}

static int compar_xy(const void* a, const void* b)
{
	vector3d* ma = (vector3d*)a;
	vector3d* mb = (vector3d*)b;
	double aa = sqrt(ma->X * ma->X + ma->Y * ma->Y);
	double bb = sqrt(mb->X * mb->X + mb->Y * mb->Y);
	return ((int)round(aa - bb));
}

void SortVectors(vector3d* arr, const int n, const int axis)
{
	if ((!arr) || (n < 1)) return;
	switch (axis) {
	case SORTVEC_3DDIST:
		qsort(arr,n,sizeof(vector3d),compar_xyz);
		break;
	case SORTVEC_BYX:
		qsort(arr,n,sizeof(vector3d),compar_x);
		break;
	case SORTVEC_BYY:
		qsort(arr,n,sizeof(vector3d),compar_y);
		break;
	case SORTVEC_BYZ:
		qsort(arr,n,sizeof(vector3d),compar_z);
		break;
	case SORTVEC_2DDIST:
		qsort(arr,n,sizeof(vector3d),compar_xy);
		break;
	}
}

void PerspectiveD(vector3d* pnt, const vector3d* fov, const vector2di* mid)
{
	if (!pnt) return;
	pnt->X = ( pnt->X * fov->X / pnt->Z ) + (double)(mid->X);
	pnt->Y = (-pnt->Y * fov->Y / pnt->Z ) + (double)(mid->Y);
}

void PerspectiveDInv(vector3d* pnt, const vector3d* fov, const vector2di* mid)
{
	if (!pnt) return;
	pnt->X = (pnt->Z * (-mid->X + pnt->X)) / (double)(fov->X);
	pnt->Y = (pnt->Z * ( mid->Y - pnt->Y)) / (double)(fov->Y);
}

void PerspectiveNInv(vector3d* pnt, const vector3d* fov, const vector2di* mid)
{
	if (!pnt) return;
	pnt->X = (2.f * (double)(mid->X) * (pnt->X - (double)(mid->X))) / (fov->X * pnt->Z);
	pnt->Y =-(2.f * (double)(mid->Y) * (pnt->Y - (double)(mid->Y))) / (fov->Y * pnt->Z);
}

void RotNormDegF(vector3d* r)
{
	while (r->X >= 360)	r->X -= 360.f;
	while (r->X < 0)	r->X += 360.f;
	while (r->Y >= 360)	r->Y -= 360.f;
	while (r->Y < 0)	r->Y += 360.f;
	while (r->Z >= 360)	r->Z -= 360.f;
	while (r->Z < 0)	r->Z += 360.f;
}

//TODO: consider this: template <class T> void RotNormDegI(CPoint3Di<T>* r)
void RotNormDegI(vector3di* r)
{
	while (r->X >= 360)	r->X -= 360;
	while (r->X < 0)	r->X += 360;
	while (r->Y >= 360)	r->Y -= 360;
	while (r->Y < 0)	r->Y += 360;
	while (r->Z >= 360)	r->Z -= 360;
	while (r->Z < 0)	r->Z += 360;
}

bool IsPntInsideCubeI(const vector3di* pnt, const vector3di* center, const int side)
{
	int x = pnt->X - center->X + side / 2;
	if ((x < 0) || (x > side)) return false;

	x = pnt->Y - center->Y + side / 2;
	if ((x < 0) || (x > side)) return false;

	x = pnt->Z - center->Z + side / 2;
	if ((x < 0) || (x > side)) return false;

	return true;
}

vector3d BilinearInterpolation(const vector3d* v, const vector3d* p)
{
	vector3d r;
	double t;

	double x1 = v[0].X;
	double x2 = v[2].X;
	if (x2 < x1) {
		t = x1;
		x1 = x2;
		x2 = t;
	} else if (x2 == x1) {
		r = v[0];
		return r;
	}

	double y1 = v[3].Y;
	double y2 = v[1].Y;
	if (y2 < y1) {
		t = y1;
		y1 = y2;
		y2 = t;
	} else if (y2 == y1) {
		r = v[1];
		return r;
	}

	double k = (x2 - x1) * (y2 - y1);
	double a = v[0].Z * (x2 - p->X) * (y2 - p->Y);
	double b = v[1].Z * (x2 - p->X) * (p->Y - y1);
	double c = v[2].Z * (p->X - x1) * (p->Y - y1);
	double d = v[3].Z * (p->X - x1) * (y2 - p->Y);

	r = *p;
	r.Z = (a + b + c + d) / k;
	return r;
}

vector2di RectangleCornerK(const int num)
{
	switch (num) {
	default:
		return (vector2di(0));
	case 1:
		return (vector2di(0,1));
	case 2:
		return (vector2di(1,1));
	case 3:
		return (vector2di(1,0));
	}
}

int FindSubRectDI(vector3di* arr, vector3di* ul, vector3di* br, vector3di* cont, const vector3di* zero, const int w, const int h)
{
	int i,j,sx,sy,ex,ey;
	unsigned l;
	bool f = false;

	//init container
	*cont = *zero;

	//move through whole array
	for (i = 0, l = 0; i < h; i++) {
		for (j = 0; j < w; j++, l++) {
			//if container is empty
			if (*cont == *zero) {
				//and point is interesting
				if (arr[l] != *zero) {
					*cont = arr[l]; //save the point
					arr[l] = *zero; //and mark its location
					sx = j;
					sy = i;
					ex = sx;
					ey = sy;
				}
			} else {
				//now check if current point is equal to our point-of-interest
				if (arr[l] == *cont) {
					arr[l] = *zero; //mark this
					if (j < sx) sx = j;
					if (j > ex) ex = j;
					if (i < sy) sy = i;
					if (i > ey) ey = i;
					f = true; //we've got at least two points
				}
			}
		}
	}

	//check the results
	if (f && (ex != sx) && (ey != sy)) {
		//our borders are good, can pass'em out
		*ul = vector3di(sx,sy,0);
		*br = vector3di(ex,ey,0);
		return 1;
	}

	//nothing can be found
	return 0;
}

/*
 * PNPOLY - Point Inclusion in Polygon Test
 * (C) W. Randolph Franklin (WRF)
 */
int pnpoly(int nvert, float *vertx, float *verty, float testx, float testy)
{
	int i,j,c = 0;
	for (i = 0, j = nvert-1; i < nvert; j = i++) {
		if ( ((verty[i]>testy) != (verty[j]>testy)) &&
				(testx < (vertx[j]-vertx[i]) * (testy-verty[i]) / (verty[j]-verty[i]) + vertx[i]) )
			c = !c;
	}
	return c;
}
