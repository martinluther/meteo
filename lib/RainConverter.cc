/*
 * RainConverter.cc -- class to convert values from one system to another
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#include <RainConverter.h>
#include <MeteoException.h>
#include <mdebug.h>

namespace meteo {

double	RainConverter::operator()(const double value, const std::string& from) const {
	if (from == getUnit())
		return value;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "converting %.2f from %s to %s",
		value, from.c_str(), getUnit().c_str());

	// convert to intermediate value (in mm)
	double	intermediate;
	if (from == "in") {
		intermediate = value * 25.4;
	} else if (from == "mm") {
		intermediate = value;
	} else {
		throw MeteoException("unknown rain unit `from'", from);
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "mm rain %.1f", intermediate);

	// convert intermediate value to target unit
	const std::string&	to = getUnit();
	if (to == "in") {
		return intermediate / 25.4;
	} else if (to == "mm") {
		return intermediate;
	} else {
		throw MeteoException("unknown rain unit `to'", getUnit());
	}
}

} /* namespace meteo */
