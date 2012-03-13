/*
 * WindConverter.cc -- class to convert values from one system to another
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#include <WindConverter.h>
#include <MeteoException.h>
#include <mdebug.h>

namespace meteo {

#define	MILE	1.609344
double	WindConverter::operator()(const double value, const std::string& from) const {
	if (from == getUnit())
		return value;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "convert %.2f from %s to %s",
		value, from.c_str(), getUnit().c_str());

	// convert to intermediate value in meters per second
	double	intermediate;
	if (from == "m/s") {
		intermediate = value;
	} else if (from == "mph") {
		intermediate = (value * MILE) / 3.6;
	} else if (from == "km/h") {
		intermediate = value / 3.6;
	} else {
		throw MeteoException("unknown wind speed unit `from'", from);
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "wind speed m/s: %.1f", intermediate);

	// convert intermediate value to target unit
	const std::string&	to = getUnit();
	if (to == "m/s") {
		return intermediate;
	} else if (to == "mph") {
		return 3.6 * intermediate / MILE;
	} else if (to == "km/h") {
		return 3.6 * intermediate;
	} else {
		throw MeteoException("unknown wind speed unit `to'", to);
	}
	return value;
}

} /* namespace meteo */
