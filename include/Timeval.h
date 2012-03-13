/*
 * Timeval.h --
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _Timeval_h
#define _Timeval_h

#include <sys/time.h>

namespace meteo {

class Timeval {
	struct timeval	t;
public:
	// construction
	Timeval(void);
	Timeval(double);
	Timeval(int);
	Timeval(struct timeval);
	~Timeval(void) { }

	// accessors
	void	now(void);	// sets to current time
	struct timeval	timeval(void) const { return t; }
	void	wait(void);
	int	getMinute(void) { return t.tv_sec / 60; }

	// conversion
	double	getValue(void) const;

	// time arithmetic
	Timeval&	operator+=(const double b);
	Timeval&	operator+=(const int b);
	friend Timeval	operator+(const Timeval& a, const Timeval& b);
	friend Timeval	operator+(const Timeval& a, const double b);
	friend Timeval	operator+(const Timeval& a, const int b);
	friend Timeval	operator-(const Timeval& a, const Timeval& b);
};

} /* namespace meteo */

#endif /* _Timeval_h */