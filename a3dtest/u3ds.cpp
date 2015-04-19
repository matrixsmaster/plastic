/*
        3DMESH Unit

        3d-Matrix_and_Vector Operations Unit

        (C) MatrixS_Master, 2008,2015
 */

#include "u3ds.h"


T3dMatrix Mtx3Mul(T3dMatrix A, T3dMatrix B)
{
	T3dMatrix res;
	res.r = 0;
	int i,j,k;
	double _s;
	for (i=0; i<4; i++)
		for (j=0; j<4; j++) {
			_s = 0.0;
			for (k=0; k<4; k++)
				_s += A.m[i][k] + B.m[k][j];
			res.m[i][j] = _s;
		}
	return res;
}

T3dPoint MtxPntMul(T3dMatrix M, T3dPoint P)
{
	T3dPoint res;
	double rk;
	for (int i=0; i<4; i++) {
		rk = P.x*M.m[0][i] + P.y*M.m[1][i] + P.z*M.m[2][i] + M.m[3][i];
		switch (i) {
		case 0: res.x = rk; break;
		case 1: res.y = rk; break;
		case 2: res.z = rk; break;
		}
	}
	return res;
}

T3dMatrix GenOMatrix()
{
	T3dMatrix res;
	res.r = 0;
	int i,j;
	for (i=0; i<4; i++)
		for (j=0; j<4; j++)
			res.m[i][j] = (i==j)? 1.0:0.0;
	return res;
}

T3dMatrix GenMtxRotX(double fi)
{
	T3dMatrix res = GenOMatrix();
	double sn = sin(fi);
	double cs = cos(fi);
	res.m[1][1] = cs;
	res.m[1][2] = sn;
	res.m[2][1] = -sn;
	res.m[2][2] = cs;
	return res;
}

T3dMatrix GenMtxRotY(double fi)
{
	T3dMatrix res = GenOMatrix();
	double sn = sin(fi);
	double cs = cos(fi);
	res.m[0][0] = cs;
	res.m[0][2] = sn;
	res.m[2][0] = -sn;
	res.m[2][2] = cs;
	return res;
}

T3dMatrix GenMtxRotZ(double fi)
{
	T3dMatrix res = GenOMatrix();
	double sn = sin(fi);
	double cs = cos(fi);
	res.m[0][0] = cs;
	res.m[0][1] = sn;
	res.m[1][0] = -sn;
	res.m[1][1] = cs;
	return res;
}

T3dPoint PntPlusPnt(T3dPoint a, T3dPoint b)
{
	T3dPoint res;
	res.x = a.x + b.x;
	res.y = a.y + b.y;
	res.z = a.z + b.z;
	return res;
}

T3dPoint PntMinusPnt(T3dPoint a, T3dPoint b)
{
	T3dPoint res;
	res.x = a.x - b.x;
	res.y = a.y - b.y;
	res.z = a.z - b.z;
	return res;
}

T3dPoint PntMinusPntAbs(T3dPoint a, T3dPoint b)
{
	T3dPoint res;
	res.x = fabs(a.x - b.x);
	res.y = fabs(a.y - b.y);
	res.z = fabs(a.z - b.z);
	return res;
}

T3dPoint PntMulPnt(T3dPoint a, T3dPoint b)
{
	T3dPoint res;
	res.x = a.x * b.x;
	res.y = a.y * b.y;
	res.z = a.z * b.z;
	return res;
}

double PntDotPnt(T3dPoint a, T3dPoint b)
{
	return (a.x*b.x + a.y*b.y + a.z*b.z);
}

T3dPoint PntMulScal(T3dPoint a, double s)
{
	T3dPoint res;
	res.x = a.x * s;
	res.y = a.y * s;
	res.z = a.z * s;
	return res;
}

double Pnt2PntLen(T3dPoint a, T3dPoint b)
{
	double dx = fabs(a.x-b.x);
	double dy = fabs(a.y-b.y);
	double dz = fabs(a.z-b.z);
	double res = sqrt(dx*dx + dy*dy + dz*dz);
	return res;
}

T3dPoint PntHalf(T3dPoint a, T3dPoint b)
{
	T3dPoint res;
	res.x = (fabs(a.x-b.x)/2) + fmin(a.x,b.x);
	res.y = (fabs(a.y-b.y)/2) + fmin(a.y,b.y);
	res.z = (fabs(a.z-b.z)/2) + fmin(a.z,b.z);
	return res;
}

T3dPoint CenterPoint(T3dPoint* pts, int n)
{
	int i;
	T3dPoint r = {0,0,0};
	if ((!pts) || (n < 1)) return r;
	for (i = 0; i < n; i++) r = PntPlusPnt(r,pts[i]);
	r = PntMulScal(r,(1.0 / (double)n));
	return r;
}

bool isPntEqual(T3dPoint a, T3dPoint b)
{
	return ((a.x==b.x)&&(a.y==b.y)&&(a.z==b.z));
}

bool isPntIntEqual(T3dPointInt a, T3dPointInt b)
{
	return ((a.x==b.x)&&(a.y==b.y)&&(a.z==b.z));
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

bool isPntRectIsect(T3dPoint aim, T3dPoint ul, T3dPoint br)
{
	if ((aim.x < ul.x) || (aim.x > br.x)) return false;
	if ((aim.y < br.y) || (aim.y > ul.y)) return false;
	return true;
}

bool isPntPoly4Isect(T3dPoint aim, T3dPoint a, T3dPoint b, T3dPoint c, T3dPoint d)
{
	float x[4],y[4];
	x[0] = a.x; y[0] = a.y;
	x[1] = b.x; y[1] = b.y;
	x[2] = c.x; y[2] = c.y;
	x[3] = d.x; y[3] = d.y;
	return (pnpoly(4,x,y,aim.x,aim.y));
}

void GenPlaneQ(int plane, T3dPoint cnt, double sz, T3dPoint* buf)
{
	int i;
	if ((!buf) || (sz <= 0)) return;
	for (i = 0; i < 4; i++) buf[i] = cnt;
	sz /= 2;
	switch (plane) {
	case 0: /* XY */
		buf[0].x -= sz;
		buf[0].y += sz;
		buf[1].x += sz;
		buf[1].y += sz;
		buf[2].x += sz;
		buf[2].y -= sz;
		buf[3].x -= sz;
		buf[3].y -= sz;
		break;
	case 1: /* YZ */
		buf[0].y -= sz;
		buf[0].z += sz;
		buf[1].y += sz;
		buf[1].z += sz;
		buf[2].y += sz;
		buf[2].z -= sz;
		buf[3].y -= sz;
		buf[3].z -= sz;
		break;
	case 2: /* ZX */
		buf[0].z -= sz;
		buf[0].x += sz;
		buf[1].z += sz;
		buf[1].x += sz;
		buf[2].z += sz;
		buf[2].x -= sz;
		buf[3].z -= sz;
		buf[3].x -= sz;
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
void GenCubeQ(T3dPoint cnt, double sz, T3dPoint* buf)
{
	T3dPoint shf;
	double h = sz / 2;
	if ((!buf) || (sz <= 0)) return;
	shf.x = 0; shf.y = -h; shf.z = 0;
	GenPlaneQ(2,PntPlusPnt(cnt,shf),sz,buf+0); //ZX0
	shf.x = -h; shf.y = 0; shf.z = 0;
	GenPlaneQ(1,PntPlusPnt(cnt,shf),sz,buf+4); //YZ1
	shf.x = 0; shf.y = 0; shf.z = h;
	GenPlaneQ(0,PntPlusPnt(cnt,shf),sz,buf+8); //XY2
	shf.x = h; shf.y = 0; shf.z = 0;
	GenPlaneQ(1,PntPlusPnt(cnt,shf),sz,buf+12); //YZ3
	shf.x = 0; shf.y = h; shf.z = 0;
	GenPlaneQ(2,PntPlusPnt(cnt,shf),sz,buf+16); //ZX4
	shf.x = 0; shf.y = 0; shf.z = -h;
	GenPlaneQ(0,PntPlusPnt(cnt,shf),sz,buf+20); //XY5
}

static int compar_x(const void* a, const void* b)
{
	T3dPoint* ma = (T3dPoint*)a;
	T3dPoint* mb = (T3dPoint*)b;
	return ((int)round(ma->x - mb->x));
}

static int compar_y(const void* a, const void* b)
{
	T3dPoint* ma = (T3dPoint*)a;
	T3dPoint* mb = (T3dPoint*)b;
	return ((int)round(ma->y - mb->y));
}

static int compar_z(const void* a, const void* b)
{
	T3dPoint* ma = (T3dPoint*)a;
	T3dPoint* mb = (T3dPoint*)b;
	return ((int)round(ma->z - mb->z));
}

static int compar_xyz(const void* a, const void* b)
{
	T3dPoint* ma = (T3dPoint*)a;
	T3dPoint* mb = (T3dPoint*)b;
	T3dPoint z = {0,0,0};
	double aa = Pnt2PntLen(z,*ma);
	double bb = Pnt2PntLen(z,*mb);
	return ((int)round(aa - bb));
}

static int compar_xy(const void* a, const void* b)
{
	T3dPoint* ma = (T3dPoint*)a;
	T3dPoint* mb = (T3dPoint*)b;
	double aa = sqrt(ma->x * ma->x + ma->y * ma->y);
	double bb = sqrt(mb->x * mb->x + mb->y * mb->y);
	return ((int)round(aa - bb));
}

void SortVectors(T3dPoint* arr, const int n, const int axis)
{
	if ((!arr) || (n < 1)) return;
	switch (axis) {
	case SORTVEC_3DDIST:
		qsort(arr,n,sizeof(T3dPoint),compar_xyz);
		break;
	case SORTVEC_BYX:
		qsort(arr,n,sizeof(T3dPoint),compar_x);
		break;
	case SORTVEC_BYY:
		qsort(arr,n,sizeof(T3dPoint),compar_y);
		break;
	case SORTVEC_BYZ:
		qsort(arr,n,sizeof(T3dPoint),compar_z);
		break;
	case SORTVEC_2DDIST:
		qsort(arr,n,sizeof(T3dPoint),compar_xy);
		break;
	}
}

T3dPoint InterpolatePntByX(T3dPoint a, T3dPoint b, double x)
{
	T3dPoint d,r;
	if (Pnt2PntLen(a,b) <= 0) return a;
//	d = PntMinusPntAbs(b,a);
	if (b.x < a.x) SWAPVEC3D(a,b,d);
	d = PntMinusPnt(b,a);
	if (d.x <= 0) return a;
//	r.x = x;
//	r.y = d.y * (x - fmin(a.x,b.x)) / d.x + fmin(a.y,b.y);
//	r.z = d.z * (x - fmin(a.x,b.x)) / d.x + fmin(a.z,b.z);
	r.x = x;
	r.y = d.y * (x - a.x) / d.x + a.y;
	r.z = d.z * (x - a.x) / d.x + a.z;
	return r;
}

T3dPoint InterpolatePntByY(T3dPoint a, T3dPoint b, double y)
{
	T3dPoint d,r;
	if (Pnt2PntLen(a,b) <= 0) return a;
//	d = PntMinusPntAbs(b,a);
	if (b.y < a.y) SWAPVEC3D(a,b,d);
	d = PntMinusPnt(b,a);
	if (d.y <= 0) return a;
//	r.x = d.x * (y - fmin(a.y,b.y)) / d.y + fmin(a.x,b.x);
//	r.y = y;
//	r.z = d.z * (y - fmin(a.y,b.y)) / d.y + fmin(a.z,b.z);
	r.x = d.x * (y - a.y) / d.y + a.x;
	r.y = y;
	r.z = d.z * (y - a.y) / d.y + a.z;
	return r;
}

T3dPoint InterpolatePntByZ(T3dPoint a, T3dPoint b, double z)
{
	T3dPoint d,r;
	if (Pnt2PntLen(a,b) <= 0) return a;
//	d = PntMinusPntAbs(b,a);
	if (b.z < a.z) SWAPVEC3D(a,b,d);
	d = PntMinusPnt(b,a);
	if (d.z <= 0) return a;
//	r.x = d.x * (z - fmin(a.z,b.z)) / d.z + fmin(a.x,b.x);
//	r.y = d.y * (z - fmin(a.z,b.z)) / d.z + fmin(a.y,b.y);
//	r.z = z;
	r.x = d.x * (z - a.z) / d.z + a.x;
	r.y = d.y * (z - a.z) / d.z + a.y;
	r.z = z;
	return r;
}

bool AxSect(const double a, const double b, const double x)
{
	double bot = fmin(a,b);
	double top = bot + fabs(a - b);
	return ((bot <= x) && (x <= top));
}

double InterpolateZQ(T3dPoint* pts, double x, double y)
{
	int i,j;
	T3dPoint r,l,c,*buf;
	if (!pts) return -1;
	buf = (T3dPoint*)malloc(sizeof(T3dPoint)*4);
	if (!buf) return -1;
	/* Find two lines intersecting YZ-plane of interest */
	for (i = 0, j = 0; (i < 4) && (j < 4); i++) {
		l = pts[i];
		r = pts[(i<3)? (i+1):0];
		if (AxSect(l.y,r.y,y)) {
			buf[j++] = l;
			buf[j++] = r;
		}
	}
	if (j == 4) {
		/* Calculate basic points on that lines */
		l = InterpolatePntByY(buf[0],buf[1],y);
		r = InterpolatePntByY(buf[2],buf[3],y);
		if ((AxSect(l.x,r.x,x)) && (!AxSect(l.z,r.z,0)))
			/* if basic line intersects XZ-plane of interest and not Z0 */
			c = InterpolatePntByX(l,r,x); /* calculate the actual intersection */
		else
			c.z = -1;
	} else
		c.z = -1; //out of view
	free(buf);
	return (c.z);
}

double InterpolateZQ2(T3dPoint* pts, double x, double y)
{
	T3dPoint mx,mn,*buf;
	double u,v;
	if (!pts) return -1;
	buf = (T3dPoint*)malloc(sizeof(T3dPoint)*4);
	if (!buf) return -1;
	memcpy(buf,pts,sizeof(T3dPoint)*4);
	/* What's going on here is the pure madness */
	SortVectors(buf,4,SORTVEC_BYY);
	mn = buf[0];
	mx = buf[3];
//	u = (mx.z == mn.z)? mx.z : (mx.y - mn.y) / fabs(mx.z - mn.z) * (y + mn.y);
	u = (mx.y == mn.y)? mx.z : fabs(mx.z - mn.z) / (mx.y - mn.y) * (y + mn.y);
	SortVectors(buf,4,SORTVEC_BYX);
	mn = buf[0];
	mx = buf[3];
//	v = (mx.z == mn.z)? mx.z : (mx.x - mn.x) / fabs(mx.z - mn.z) * (x + mn.x);
	v = (mx.y == mn.y)? mx.z : fabs(mx.z - mn.z) / (mx.x - mn.x) * (x + mn.x);
	free(buf);
	return ((u+v)/2);
}
