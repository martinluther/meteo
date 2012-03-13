/*
 * Scale.cc -- implementation scales of vertical axes of a Graphwindow
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: Scale.cc,v 1.2 2004/02/25 23:48:05 afm Exp $
 */
#include <Scale.h>
#include <Configuration.h>
#include <mdebug.h>
#include <MeteoException.h>

namespace meteo {

// Scale::Scale -- construct Scale object from the configuration based on
//                 an xpath and a data set (this takes extremal values into
//                 account)
Scale::Scale(const std::string& xpath, const Dataset& ds) {
	Configuration	conf;
	double		min, max, max2, min2, r, step;
	std::string	minname, maxname;
	// get the type from the configuration file
	std::string	type = conf.getString(xpath + "/@type", "static");

	// retrieve basic parameters
	min2 = conf.getDouble(xpath + "/@min", 0.);
	max2 = conf.getDouble(xpath + "/@max", 1.);
	r = max2 - min2;
	a = 1/r;

	// the attributes min and max make up the scale (in the static case)
	if (type == "static") {
		b = min2;
		return;
	}

	// all the dynamic ranges are in relation to the step height, so we
	// try to get a value for the step
	step = conf.getDouble(xpath + "/@step", 1.);

	// the dynamic range adapts to the data to be displayed
	maxname = conf.getString(xpath + "/@maxname", "none");
	if (maxname == "none") {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "no maxname specified");
		if (!conf.xpathExists(xpath + "/@max"))
			throw MeteoException("no maxname/max specified", xpath);
		max = conf.getDouble(xpath + "/@max", 0.);
	} else {
		max = ds.getData(maxname).max();
	}
	minname = conf.getString(xpath + "/@minname", "none");
	if (minname == "none") {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "no minname specified");
		if (!conf.xpathExists(xpath + "/@min"))
			throw MeteoException("no minname specified", xpath);
		min = conf.getDouble(xpath + "/@min", 0.);
	} else {
		min = ds.getData(minname).min();
	}
	if (type == "dynamic") {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "dynamic range %f - %f",
			min, max);
		b = min;
		// make sure the difference is at least the step height, 
		// increasing the maximum if necessary (this is ok for all
		// meteo data I can think of, but other applications may
		// have different needs)
		if ((max - min) < step)
			max = min + step;
		a = 1/(max - min);
		return;
	}

	// the following types all use a range derived from max and min,
	// we assume the user is clever enough to make sure they are
	// large than the step.

	// a range type scale keeps the same
	if (type == "toprange") {
		// max > max2: use the max data as the top border
		if (max > max2) {
			b = max - r; return;
		}
		// data becomes smaller than we thought in the conf file
		if (min < min2) {
			if (min + r < max) {
				// data does not fit in range
				b = max - r; return;
			} else {
				// data fits in range, go no lower than min
				b = min;
			}
		}
		b = min2; return;
	}
	if (type == "bottomrange") {
		// min < min2: use the min data as the top border
		if (min < min2) {
			b = min; return;
		}
		// data becomes larger than we thought in the conf file
		if (max > max2) {
			if (max - r > min) {
				// data does not fit in range, follow min
				b = min; return;
			} else {
				// data fits in range, go no higher than max2
				b = max - r;
			}
		}
		b = min2; return;
	}
}

} /* namespace meteo */
