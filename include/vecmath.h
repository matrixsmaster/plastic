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

#ifndef VECMATH_H_
#define VECMATH_H_

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

class CPoint3D {
public:
	CPoint3D() : 								X(0), Y(0), Z(0), epsilon(0) {}
	CPoint3D(double x, double y, double z) :	X(x), Y(y), Z(z), epsilon(0) {}
	CPoint3D(double all) :						X(all), Y(all), Z(all), epsilon(0) {}
	CPoint3D(const CPoint3D & nw) :				X(nw.X), Y(nw.Y), Z(nw.Z), epsilon(0) {}
	virtual ~CPoint3D();
	CPoint3D & operator = (const CPoint3D & old) 		{ X=old.X; Y=old.Y; Z=old.Z; return *this; }
	CPoint3D operator + (const CPoint3D & add) const	{return CPoint3D((X+add.X),(Y+add.Y),(Z+add.Z));}
	CPoint3D operator - (const CPoint3D & sub) const	{return CPoint3D((X-sub.X),(Y-sub.Y),(Z-sub.Z));}
	CPoint3D & operator += (const double add)			{ X+=add; Y+=add; Z+=add; return *this; }
	CPoint3D & operator -= (const double sub)			{ X-=sub; Y-=sub; Z-=sub; return *this; }
	CPoint3D & operator += (const CPoint3D add)			{ X+=add.X; Y+=add.X; Z+=add.Z; return *this; }
	CPoint3D & operator -= (const CPoint3D sub)			{ X-=sub.Y; Y-=sub.Y; Z-=sub.Z; return *this; }
	CPoint3D & operator *= (const double fmul)			{ X*=fmul; Y*=fmul; Z*=fmul; return *this; }
	CPoint3D & operator /= (const double fdiv)			{ X/=fdiv; Y/=fdiv; Z/=fdiv; return *this; }
	CPoint3D operator * (const double fmul) const		{ return CPoint3D((X*fmul),(Y*fmul),(Z*fmul)); }
	CPoint3D operator / (const double fdiv) const		{ return CPoint3D((X/fdiv),(Y/fdiv),(Z/fdiv)); }

	bool operator == (const CPoint3D & cmp)
			{ return ((fcompare(X,cmp.X)==0) && (fcompare(Y,cmp.Y)==0) && (fcompare(Z,cmp.Z)==0)); }

	bool operator != (const CPoint3D & cmp)
			{ return ((fcompare(X,cmp.X)!=0) || (fcompare(Y,cmp.Y)!=0) || (fcompare(Z,cmp.Z)!=0)); }

	double Module() const								{ return sqrt(X*X + Y*Y + Z*Z); }
	double DotProduct(const CPoint3D & s) const 		{ return X*s.X + Y*s.Y + Z*s.Z; }
	CPoint3D MinusAbs(const CPoint3D & b) const			{ return CPoint3D(fabs(X-b.X),fabs(Y-b.Y),fabs(Z-b.Z)); }
	CPoint3D HalfWay(const CPoint3D & s);
	double DistanceTo(const CPoint3D & s);
	int fcompare(const double a, const double b);
	double X, Y, Z, epsilon;
};

typedef CPoint3D vector3d;


#endif /* VECMATH_H_ */
