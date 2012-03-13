/*
 * Axis.h -- abstracts graphs inside windows
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: Axis.h,v 1.2 2004/02/25 23:52:34 afm Exp $
 */
#ifndef _Axis_h
#define _Axis_h

#include <Scale.h>

namespace meteo {

class Axis {
	std::string	format;
	double		first, last, step;
	bool		gridlines, ticks;
public:
	Axis(void) : format("%.0f") {	
		first = 0.; last = 1.; step = 1.;
		gridlines = false; ticks = false;
	}
	Axis(std::string& f, double fi, double la, double st) : format(f) {
		first = fi; last = la; step = st;
	}
	Axis(const std::string& xpath, const Scale& sc);
	~Axis(void) { }
	void	setFirst(double f) { first = f; }
	void	setLast(double l) { last = l; }
	void	setStep(double s) { step = s; }
	void	setFormat(const std::string& f) { format = f; }
	void	setGridlines(bool g) { gridlines = g; }
	void	setTicks(bool t) { ticks = t; }

	double	getFirst(void) const { return first; }
	double	getLast(void) const { return last; }
	double	getStep(void) const { return step; }
	const std::string& 	getFormat(void) const { return format; }
	bool	getGridlines(void) const { return gridlines; }
	bool	getTicks(void) const { return ticks; }
};

} /* namespace meteo */

#endif /* _Axis_h */
