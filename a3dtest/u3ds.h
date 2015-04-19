/*
        3DMESH Unit

        3d-Matrix_and_Vector Operations Unit

        (C) MatrixS_Master, 2008,2015
 */

#ifndef u3dsH
#define u3dsH

#include <math.h>
#include <stdlib.h>
#include <string.h>


#define SORTVEC_2DDIST	4
#define SORTVEC_3DDIST	0
#define SORTVEC_BYX		1
#define SORTVEC_BYY		2
#define SORTVEC_BYZ		3

typedef struct A3dPoint {
	double x,y,z;
} T3dPoint;

typedef struct A3dPointInt {
	int x,y,z;
} T3dPointInt;

//typedef struct ATriFace {
//	unsigned int a,b,c;
//} TTriFace;

typedef struct A3dMatrix {
	double m[4][4];
	int r;
} T3dMatrix;

#define SWAPVEC3D(A,B,Sw) { Sw = A; A = B; B = Sw; }

T3dMatrix Mtx3Mul(T3dMatrix A, T3dMatrix B);
T3dPoint MtxPntMul(T3dMatrix M, T3dPoint P);

T3dMatrix GenOMatrix();
T3dMatrix GenMtxRotX(double fi);
T3dMatrix GenMtxRotY(double fi);
T3dMatrix GenMtxRotZ(double fi);

T3dPoint PntPlusPnt(T3dPoint a, T3dPoint b);
T3dPoint PntMinusPnt(T3dPoint a, T3dPoint b);
T3dPoint PntMinusPntAbs(T3dPoint a, T3dPoint b);
T3dPoint PntMulPnt(T3dPoint a, T3dPoint b);
double PntDotPnt(T3dPoint a, T3dPoint b);
T3dPoint PntMulScal(T3dPoint a, double s);
double Pnt2PntLen(T3dPoint a, T3dPoint b);
T3dPoint PntHalf(T3dPoint a, T3dPoint b);
T3dPoint CenterPoint(T3dPoint* pts, int n);
bool AxSect(const double a, const double b, const double x);
T3dPoint InterpolatePntByX(T3dPoint a, T3dPoint b, double x);
T3dPoint InterpolatePntByY(T3dPoint a, T3dPoint b, double y);
T3dPoint InterpolatePntByZ(T3dPoint a, T3dPoint b, double z);
double InterpolateZQ(T3dPoint* pts, double x, double y);
double InterpolateZQ2(T3dPoint* pts, double x, double y);

void SortVectors(T3dPoint* arr, const int n, const int axis);

bool isPntEqual(T3dPoint a, T3dPoint b);
bool isPntIntEqual(T3dPointInt a, T3dPointInt b);

int pnpoly(int nvert, float *vertx, float *verty, float testx, float testy);
bool isPntRectIsect(T3dPoint aim, T3dPoint ul, T3dPoint br);
bool isPntPoly4Isect(T3dPoint aim, T3dPoint a, T3dPoint b, T3dPoint c, T3dPoint d);
#define isPntPoly4IsectA(A,B) isPntPoly4Isect(A,B[0],B[1],B[2],B[3])

void GenPlaneQ(int plane, T3dPoint cnt, double sz, T3dPoint* buf);
void GenCubeQ(T3dPoint cnt, double sz, T3dPoint* buf);

#endif
