/*
 * Timeval.cc -- time arithmetic
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: Timeval.cc,v 1.7 2009/01/10 19:00:25 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <Timeval.h>
#ifdef HAVE_MATH_H
#include <math.h>
#endif /* HAVE_MATH_H */
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif /* HAVE_SYS_TYPES_H */
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

time_t	Timeval::getTimekey(void) const {
	time_t	result = t.tv_sec;
	result -= (result % 60);
	return result;
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
