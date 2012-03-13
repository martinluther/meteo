/*
 * Timeval.cc -- time arithmetic
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#include <Timeval.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <mdebug.h>

namespace meteo {

Timeval::Timeval(void) {
	t.tv_sec = t.tv_usec = 0;
}

Timeval::Timeval(struct timeval tv) {
	t.tv_sec = tv.tv_sec;
	t.tv_usec = tv.tv_usec;
}

Timeval::Timeval(double r) {
	t.tv_sec = (int)r;
	t.tv_usec = (int)(1000000. * (r - t.tv_sec));
}

Timeval::Timeval(int l) {
	t.tv_sec = l; t.tv_usec = 0;
}

void	Timeval::now(void) {
	gettimeofday(&t, NULL);
}

void	Timeval::wait(void) {
	struct timeval	local;
	local = t;
	if (0 != select(0, NULL, NULL, NULL, &local)) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, MDEBUG_ERRNO,
			"select problem in drain");
	}
}

double	Timeval::getValue(void) const {
	return t.tv_sec + t.tv_usec/1000000.;
}

Timeval&	Timeval::operator+=(double b) {
	t.tv_sec += (int)b;
	t.tv_usec += (int)(1000000 * (b - (int)b));
	if (t.tv_usec >= 1000000) {
		t.tv_usec -= 1000000;
		t.tv_sec++;
	}
	return *this;
}

Timeval&	Timeval::operator+=(int b) {
	t.tv_sec += b;
	return *this;
}

Timeval	operator+(const Timeval& a, const Timeval& b) {
	struct timeval	tv;
	tv.tv_sec = a.t.tv_sec + b.t.tv_sec;
	tv.tv_usec = a.t.tv_usec + b.t.tv_usec;
	if (tv.tv_usec >= 1000000) {
		tv.tv_sec++;
		tv.tv_usec -= 1000000;
	}
	return Timeval(tv);
}

Timeval	operator+(const Timeval& a, const double b) {
	return Timeval(a.getValue() + b);
}

Timeval	operator+(const Timeval& a, const int b) {
	struct timeval	tv;
	tv.tv_sec = a.t.tv_sec + b; tv.tv_usec = a.t.tv_usec;
	return Timeval(tv);
}

Timeval	operator-(const Timeval& a, const Timeval& b) {
	return Timeval(a.getValue() - b.getValue());
}

} /* namespace meteo */