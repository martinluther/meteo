/*
 * Vector.cc -- Vector arithmetic
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#include <Vector.h>
#include <math.h>
#include <mdebug.h>

namespace meteo {

double	Vector::getArg(void) const {
	if (getAbs() == 0.) {
		return 0.;
	}
	return atan2(y, x);
}

double	Vector::getAzi(void) const {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "compute azi of vector (%.1f, %.1f)",
		x, y);
	double	a = 180. * getArg() / 3.1415926535;
	a = 90 - a;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "raw azi %.1f", a);
	if (a < 0.)
		return a + 360.;
	if (a > 360.)
		return a - 360.;
	return a;
}

double	Vector::getAbs(void) const {
	return sqrt(x*x + y*y);
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

