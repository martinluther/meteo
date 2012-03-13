/*
 * Vector.h -- encapsulates vector operations for wind
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: Vector.h,v 1.9 2004/02/25 23:52:35 afm Exp $
 */
#ifndef _Vector_h
#define _Vector_h

#include <math.h>

namespace meteo {

#define PI	3.1415926535

class Vector {
	double	x, y;
public:
	// construction and destruction
	Vector(double xx, double yy);
	Vector(double r, unsigned short azideg, bool azinotarg);
	Vector(void) { x = 0.; y = 0.; }
	~Vector(void) { }

	// accessors
	double	getX(void) const { return x; }
	double	getY(void) const { return y; }
	double	getArg(void) const;	// angles always in radians
	double	getAzi(void) const;	// angles always in radians
	double	getAzideg(void) const;	// in degrees, for display strings
	double	getAbs(void) const;

	// += operator
	Vector&	operator+=(const Vector& a);
	Vector& operator*=(const double a);

	// computation
	friend Vector	operator+(const Vector& a, const Vector& b);
	friend Vector	operator-(const Vector& a, const Vector& b);
	friend Vector	operator*(const double a, const Vector& b);
};

} /* namespace meteo */

#endif /* _Vector_h */
