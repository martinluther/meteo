/*
 * Axis.cc -- axis implementation for graphing
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: Axis.cc,v 1.2 2004/02/25 23:48:04 afm Exp $
 */
#include <Axis.h>
#include <Configuration.h>
#include <mdebug.h>

namespace meteo {

// Axis::Axis -- construct a Axis object from the configuration based on an
//               Xpath in the configuration file
#define	MINORIGIN	-1000000000.
Axis::Axis(const std::string& xpath, const Scale& sc) {
	Configuration	conf;
	// the attributes first, last, step and format and the flags ticks
	// and gridlines make up the axis obj
	std::string	type = conf.getString(xpath + "/@type", "static");
	format = conf.getString(xpath + "/@format", "%f");
	first = conf.getDouble(xpath + "/@first", 0.);
	last = conf.getDouble(xpath + "/@last", 0.);
	step = conf.getDouble(xpath + "/@step", 1.);
	ticks = conf.getBool(xpath + "/@ticks", false);
	gridlines = conf.getBool(xpath + "/@gridlines", false);

	// for the static/default Axis definition, this is all we need
	if (type == "static") {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "static configuration");
		return;
	}

	// compute a suitable origin (if not set explicitely in the config file
	double	origin = conf.getDouble(xpath + "/@origin", MINORIGIN);
	if (origin == MINORIGIN) {
		origin = first - floor(first/step) * step;
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "orgin: %f", origin);
	}

	// we want at most maxtickcount ticks, but the step should be
	// of the form step * 10^n * x, where x \in {1,2,5}, and n is
	// non-negative
	if (type == "dynamic") {
		// compute a suitable step, i.e. the number of ticks should
		// be as large as possible with the current ticklimit

		// the step is a multiple of step of one of the following
		// forms 
		// 	    10^b * step
		//	2 * 10^b * step
		//	5 * 10^b * step
		int	maxtickcount = conf.getInt(xpath + "/@maxtickcount",
						10);
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "maxtickcount = %d",
			maxtickcount);
		double	s = ceil(log10(sc.range()/(step * maxtickcount)));
		if (s < 0) s = 0.;	// make sure step does not become
					// smaller than the configured step
		double	stepbase = pow(10., s);
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "initial step = %.f, "
			"stepbase = %f", step, stepbase);
		step *= stepbase;
		// if the step is not smaller than the configured step, i.e.
		// it is at least 10 times larger, we can make the step 
		// smaller without exceeding the maximum allowed ticks.
		if (s > 0.5) {
			if ((5 * sc.range() / step) < maxtickcount) {
				step = step / 5.;
			}
			if ((2 * sc.range() / step) < maxtickcount) {
				step = step / 2.;
			}
		}
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "final step = %f", step);
	}

	// compute first and last anew, they must be multiples of the step
	last = origin + step * floor((sc.getMax() - origin)/step);
	first = origin - step * floor((origin - sc.getMin())/step);
}
} /* namespace meteo */
