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

template <class T> class CPoint2D {
public:
	CPoint2D() : 								X(0), Y(0) {}
	CPoint2D(T X, T Y) : 						X(X), Y(Y) {}
	CPoint2D(T both) : 							X(both), Y(both) {}
	CPoint2D(const CPoint2D & nw) : 			X(nw.X), Y(nw.Y) {}
	virtual ~CPoint2D()							{}
	CPoint2D & operator = (const CPoint2D & other) 		{ X=other.X; Y=other.Y; return *this; }
	CPoint2D operator + (const CPoint2D & add) const 	{ return CPoint2D((X+add.X),(Y+add.Y)); }
	CPoint2D operator - (const CPoint2D & sub) const 	{ return CPoint2D((X-sub.X),(Y-sub.Y)); }
	CPoint2D & operator += (const T add) 				{ X+=add; 	Y+=add; return *this; }
	CPoint2D & operator -= (const T sub) 				{ X-=sub; 	Y-=sub; return *this; }
	CPoint2D & operator += (const CPoint2D add)	 		{ X+=add.X; Y+=add.Y; return *this; }
	CPoint2D & operator -= (const CPoint2D sub) 		{ X-=sub.X; Y-=sub.Y; return *this; }
	CPoint2D & operator *= (const T imul) 				{ X*=imul; 	Y*=imul; return *this; }
	CPoint2D & operator /= (const T idiv) 				{ X/=idiv; 	Y/=idiv; return *this; }
	CPoint2D operator * (const T imul) const 			{ return CPoint2D((X*imul),(Y*imul)); }
	CPoint2D operator / (const T idiv) const 			{ return CPoint2D((X/idiv),(Y/idiv)); }
	bool operator == (const CPoint2D & comp) const 		{ return ((X==comp.X) && (Y==comp.Y)); }
	bool operator != (const CPoint2D & comp) const 		{ return ((X!=comp.X) || (Y!=comp.Y)); }
	double Module() const 								{ return sqrt(X*X + Y*Y); }
	T Max() const										{ return ((X>Y)? X:Y); }
	T X, Y;
};

typedef CPoint2D<int> vector2di;


/* CPoint3D is basically for a real values, so we don't make a template of it */
class CPoint3D {
public:
	CPoint3D() : 								X(0), Y(0), Z(0), epsilon(0) {}
	CPoint3D(double X, double Y, double Z) :	X(X), Y(Y), Z(Z), epsilon(0) {}
	CPoint3D(double all) :						X(all), Y(all), Z(all), epsilon(0) {}
	CPoint3D(const CPoint3D & nw) :				X(nw.X), Y(nw.Y), Z(nw.Z), epsilon(0) {}
	virtual ~CPoint3D()							{}
	CPoint3D & operator = (const CPoint3D & old) 		{ X=old.X; Y=old.Y; Z=old.Z; return *this; }
	CPoint3D operator + (const CPoint3D & add) const	{return CPoint3D((X+add.X),(Y+add.Y),(Z+add.Z));}
	CPoint3D operator - (const CPoint3D & sub) const	{return CPoint3D((X-sub.X),(Y-sub.Y),(Z-sub.Z));}
	CPoint3D & operator += (const double add)			{ X+=add; 	Y+=add; 	Z+=add; return *this; }
	CPoint3D & operator -= (const double sub)			{ X-=sub; 	Y-=sub; 	Z-=sub; return *this; }
	CPoint3D & operator += (const CPoint3D add)			{ X+=add.X; Y+=add.Y; 	Z+=add.Z; return *this; }
	CPoint3D & operator -= (const CPoint3D sub)			{ X-=sub.X; Y-=sub.Y; 	Z-=sub.Z; return *this; }
	CPoint3D & operator *= (const double fmul)			{ X*=fmul; 	Y*=fmul; 	Z*=fmul; return *this; }
	CPoint3D & operator /= (const double fdiv)			{ X/=fdiv; 	Y/=fdiv; 	Z/=fdiv; return *this; }
	CPoint3D & operator *= (const CPoint3D mul)			{ X*=mul.X; Y*=mul.Y; 	Z*=mul.Z; return *this; }
	CPoint3D & operator /= (const CPoint3D div)			{ X/=div.X; Y/=div.Y; 	Z/=div.Z; return *this; }
	CPoint3D operator * (const double fmul) const		{ return CPoint3D((X*fmul),(Y*fmul),(Z*fmul)); }
	CPoint3D operator / (const double fdiv) const		{ return CPoint3D((X/fdiv),(Y/fdiv),(Z/fdiv)); }
	CPoint3D operator * (const CPoint3D & mul) const	{ return CPoint3D((X*mul.X),(Y*mul.Y),(Z*mul.Z)); }
	CPoint3D operator / (const CPoint3D & div) const	{ return CPoint3D((X/div.X),(Y/div.Y),(Z/div.Z)); }

	bool operator == (const CPoint3D & cmp)
			{ return ((fcompare(X,cmp.X)==0) && (fcompare(Y,cmp.Y)==0) && (fcompare(Z,cmp.Z)==0)); }

	bool operator != (const CPoint3D & cmp)
			{ return ((fcompare(X,cmp.X)!=0) || (fcompare(Y,cmp.Y)!=0) || (fcompare(Z,cmp.Z)!=0)); }

	double Module() const								{ return sqrt(X*X + Y*Y + Z*Z); }
	double DotProduct(const CPoint3D & s) const 		{ return X*s.X + Y*s.Y + Z*s.Z; }
	CPoint3D MinusAbs(const CPoint3D & b) const			{ return CPoint3D(fabs(X-b.X),fabs(Y-b.Y),fabs(Z-b.Z)); }
	CPoint3D HalfWay(const CPoint3D s);
	double DistanceTo(const CPoint3D s);
	int fcompare(const double a, const double b);
	double X, Y, Z, epsilon;
};

typedef CPoint3D vector3d;


/* CPoint3D strict version for integer values, made as a template */
template <class T> class CPoint3Di {
public:
	CPoint3Di() : 								X(0), Y(0), Z(0) {}
	CPoint3Di(T X, T Y, T Z) :					X(X), Y(Y), Z(Z) {}
	CPoint3Di(T all) :							X(all), Y(all), Z(all) {}
	CPoint3Di(const CPoint3Di & nw) :			X(nw.X), Y(nw.Y), Z(nw.Z) {}
	virtual ~CPoint3Di()						{}
	CPoint3Di & operator = (const CPoint3Di & old) 		{ X=old.X; Y=old.Y; Z=old.Z; return *this; }
	CPoint3Di operator + (const CPoint3Di & add) const	{ return CPoint3Di((X+add.X),(Y+add.Y),(Z+add.Z)); }
	CPoint3Di operator - (const CPoint3Di & sub) const	{ return CPoint3Di((X-sub.X),(Y-sub.Y),(Z-sub.Z)); }
	CPoint3Di & operator += (const T add)				{ X+=add; 	Y+=add; 	Z+=add; return *this; }
	CPoint3Di & operator -= (const T sub)				{ X-=sub; 	Y-=sub; 	Z-=sub; return *this; }
	CPoint3Di & operator += (const CPoint3Di add)		{ X+=add.X; Y+=add.Y; 	Z+=add.Z; return *this; }
	CPoint3Di & operator -= (const CPoint3Di sub)		{ X-=sub.X; Y-=sub.Y; 	Z-=sub.Z; return *this; }
	CPoint3Di & operator *= (const T imul)				{ X*=imul; 	Y*=imul; 	Z*=imul; return *this; }
	CPoint3Di & operator /= (const T idiv)				{ X/=idiv; 	Y/=idiv; 	Z/=idiv; return *this; }
	CPoint3Di & operator *= (const CPoint3Di mul)		{ X*=mul.X; Y*=mul.Y; 	Z*=mul.Z; return *this; }
	CPoint3Di & operator /= (const CPoint3Di div)		{ X/=div.X; Y/=div.Y; 	Z/=div.Z; return *this; }
	CPoint3Di operator * (const T imul) const			{ return CPoint3Di((X*imul),(Y*imul),(Z*imul)); }
	CPoint3Di operator / (const T idiv) const			{ return CPoint3Di((X/idiv),(Y/idiv),(Z/idiv)); }
	CPoint3Di operator * (const CPoint3Di & mul) const	{ return CPoint3Di((X*mul.X),(Y*mul.Y),(Z*mul.Z)); }
	CPoint3Di operator / (const CPoint3Di & div) const	{ return CPoint3Di((X/div.X),(Y/div.Y),(Z/div.Z)); }
	bool operator == (const CPoint3Di & comp) const 	{ return ((X==comp.X) && (Y==comp.Y) && (Z==comp.Z)); }
	bool operator != (const CPoint3Di & comp) const 	{ return ((X!=comp.X) || (Y!=comp.Y) || (Z!=comp.Z)); }
	CPoint3D ToReal()									{ return CPoint3D((double)X,(double)Y,(double)Z); }
	CPoint3Di normalize()								{ double a = sqrt(X*X + Y*Y + Z*Z); return CPoint3Di(round(X/a), round(Y/a), round(Z/a));}
	T Max()												{ CPoint2D<T> a(X,Y); a = CPoint2D<T>(a.Max(),Z); return a.Max(); }
	T X, Y, Z;
};

/* Some widely used cases */
typedef CPoint3Di<int> vector3di;
typedef CPoint3Di<long long int> vector3dlli;
typedef CPoint3Di<unsigned long long int> vector3dulli;


#endif /* VECMATH_H_ */
