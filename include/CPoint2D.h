/*
 * CPoint2D.h
 *  Created on: 06.12.2012
 *
 *	Authors:
 *	Soloviov Dmitry aka matrixsmaster
 *
 */

#ifndef CPOINT2D_H_
#define CPOINT2D_H_

#include <math.h>

class CPoint2D {
public:
	CPoint2D() : X(0), Y(0) {}
	CPoint2D(int x, int y) : X(x), Y(y) {}
	CPoint2D(int both) : X(both), Y(both) {}
	CPoint2D(const CPoint2D & nw) : X(nw.X), Y(nw.Y) {}
	virtual ~CPoint2D();
	CPoint2D & operator = (const CPoint2D & other) { X=other.X; Y=other.Y; return *this; }
	CPoint2D operator + (const CPoint2D & add) const {return CPoint2D((X+add.X),(Y+add.Y));}
	CPoint2D operator - (const CPoint2D & sub) const {return CPoint2D((X-sub.X),(Y-sub.Y));}
	CPoint2D & operator += (const int add) { X+=add; Y+=add; return *this; }
	CPoint2D & operator -= (const int sub) { X-=sub; Y-=sub; return *this; }
	CPoint2D & operator += (const CPoint2D add) { X+=add.X; Y+=add.X; return *this; }
	CPoint2D & operator -= (const CPoint2D sub) { X-=sub.Y; Y-=sub.Y; return *this; }
	CPoint2D & operator *= (const int imul) { X*=imul; Y*=imul; return *this; }
	CPoint2D & operator /= (const int idiv) { X/=idiv; Y/=idiv; return *this; }
	CPoint2D operator * (const int imul) const { return CPoint2D((X*imul),(Y*imul)); }
	CPoint2D operator / (const int idiv) const { return CPoint2D((X/idiv),(Y/idiv)); }
	bool operator == (const CPoint2D & comp) const {return ((X==comp.X) && (Y==comp.Y));}
	bool operator != (const CPoint2D & comp) const {return ((X!=comp.X) || (Y!=comp.Y));}
	double Module() const { return sqrt(X*X + Y*Y); }
	int X;
	int Y;
};

typedef CPoint2D vector2di;

#endif /* CPOINT2D_H_ */
