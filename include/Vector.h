/*
 * Vector.h -- encapsulates vector operations for wind
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _Vector_h
#define _Vector_h

#include <math.h>

namespace meteo {

class Vector {
	double	x, y;
public:
	// construction and destruction
	Vector(double xx, double yy) { x = xx; y = yy; }
	Vector(double r, unsigned short azi) {
		double	p = 3.1415926535 * azi/360.;
		this->x = r * cos(p);
		this->y = r * sin(p);
	}
	Vector(void) { x = 0.; y = 0.; }
	~Vector(void) { }

	// accessors
	double	getX(void) const { return x; }
	double	getY(void) const { return y; }
	double	getArg(void) const;
	double	getAzi(void) const;
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
