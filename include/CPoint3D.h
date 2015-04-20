/*
 * CPoint3D.h
 *
 *  Created on: Apr 20, 2015
 *      Author: matrixsmaster
 */

#ifndef CPOINT3D_H_
#define CPOINT3D_H_

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

#endif /* CPOINT3D_H_ */
