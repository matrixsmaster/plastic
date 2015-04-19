/*
 * CPoint2D.cpp
 *  Created on: 06.12.2012
 *
 *	Authors:
 *	Soloviov Dmitry aka matrixsmaster
 *
 */

#include "CPoint2D.h"
#include <math.h>

double CPoint2D::Module()
{
	return sqrt(X*X + Y*Y);
}
