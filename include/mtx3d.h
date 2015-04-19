/*
 * mtx3d.h
 *
 *  Created on: Apr 19, 2015
 *      Author: matrixsmaster
 */

#ifndef MTX3D_H_
#define MTX3D_H_

struct SMatrix3d {
	double m[4][4];
	int r;
};

SMatrix3d GenOMatrix();
SMatrix3d GenMtxRotX(const double fi);
SMatrix3d GenMtxRotY(const double fi);
SMatrix3d GenMtxRotZ(const double fi);

SMatrix3d Mtx3Mul(const SMatrix3d A, const SMatrix3d B);
SMatrix3d MtxPntMul(const SMatrix3d M, const SMatrix3d P);

#endif /* MTX3D_H_ */
