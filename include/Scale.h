/*
 * Scale.h -- abstracts scales for vertical axis of a GraphWindow
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _Scale_h
#define _Scale_h

#include <Scale.h>
#include <Dataset.h>
#include <string>

namespace meteo {

// Scale objects are used to convert application domain data into data useful
// for graphing. After the object has been initialized with the min and max
// values, it will always return a value between 0 and 1, so that 0 corresponds
// to the minimum, 1 corresponds to the maximum
class Scale {
	double  a, b;
public:
	Scale(void) { a = 1.; b = 0.; } // if nothing is specified, use id map
	Scale(double min, double max) {
		a = 1/(max - min);
		b = min;
	}
	Scale(const std::string& xpath, const Dataset& ds);
	~Scale(void) { }
	double  operator()(double x) const {
		double	y = a * (x - b);
		if (y > 1.) return 1.;
		if (y < 0.) return 0.;
		return y;
	}
	double	getMin(void) const { return b; }
	double	getMax(void) const { return b + 1/a; }
	double	range(void) const { return 1/a; }
};

} /* namespace meteo */

#endif /* _Scale_h */
