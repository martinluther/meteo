/*
 * Vector.cc -- Vector arithmetic
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: Vector.cc,v 1.11 2004/02/25 23:48:06 afm Exp $
 */
#include <Vector.h>
#include <math.h>
#include <mdebug.h>

namespace meteo {

Vector::Vector(double xx, double yy) {
	if ((fabs(xx) > 1000.) || (fabs(yy) > 1000.))
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "illegally large components "
			"in constructor", xx, yy);
	x = xx; y = yy;
}

Vector::Vector(double r, unsigned short azideg, bool azinotarg) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "creating Vector from r = %f, "
		"azideg =%f", r, azideg);
	double  p = PI * azideg/180.;
	if (azinotarg) {
		y = r * cos(p);
		x = r * sin(p);
	} else {
		x = r * cos(p);
		y = r * sin(p);
	}
	if ((fabs(x) > 1000.) || (fabs(y) > 1000.)) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "illegal vector components");
	}
}

double	Vector::getArg(void) const {
	if (getAbs() < 0.0001) {
		return 0.;
	}
	return ::atan2(y, x);
}

double	Vector::getAzi(void) const {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "compute azi of vector (%.1f, %.1f)",
		x, y);
	// don't care if the absolute value is small enough
	if (getAbs() < 0.0001)
		return 0.;
	double	a = ::atan2(x, y);
	while (a < 0.)
		a += 2 * PI;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "raw azi %.1f (%.fdeg)", a,
		180.*(a/PI));
	return a;
}

double	Vector::getAzideg(void) const {
	return 180. * getAzi()/PI;
}

double	Vector::getAbs(void) const {
	return ::sqrt(x*x + y*y);
}

Vector&	Vector::operator+=(const Vector& a) {
	this->x += a.x;
	this->y += a.y;
	return *this;
}
Vector&	Vector::operator*=(const double a) {
	this->x *= a;
	this->y *= a;
	return *this;
}

Vector	operator+(const Vector& a, const Vector& b) {
	return Vector(a.x + b.x, a.y + b.y);
}

Vector	operator-(const Vector& a, const Vector& b) {
	return Vector(a.x - b.x, a.y - b.y);
}

Vector	operator*(const double a, const Vector& b) {
	return Vector(a * b.x, a * b.y);
}

} /* namespace meteo */

