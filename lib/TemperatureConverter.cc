/*
 * TemperatureConverter.cc -- class to convert values from one system to another
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: TemperatureConverter.cc,v 1.2 2004/02/25 23:48:05 afm Exp $
 */
#include <TemperatureConverter.h>
#include <MeteoException.h>
#include <mdebug.h>

namespace meteo {

double	TemperatureConverter::operator()(const double value,
	const std::string& from) const {
	if (from == getUnit())
		return value;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "converting %.2f from %s to %s",
		value, from.c_str(), getUnit().c_str());

	// convert to intermediate value
	double	intermediate;
	if ((from == "degrees Fahrenheit") || (from == "degF")
		|| (from == "F")) {
		intermediate = 273.15 + (5. * (value - 32.) / 9.);
	} else if ((from == "degrees Celsius") || (from == "degC")
		|| (from == "C")) {
		intermediate = value + 273.15;
	} else if ((from == "Kelvin") || (from == "K")) {
		intermediate = value;
	} else {
		throw MeteoException("unknown temperature unit `from'", from);
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "absolute temperature %.1f",
		intermediate);

	// convert to target value
	const std::string&	to = getUnit();
	if ((to == "degrees Fahrenheit") || (to == "degF") || (to == "F")) {
		return 9. * (intermediate - 273.15) / 5. + 32.;
	} else if ((to == "degrees Celsius") || (to == "degC") || (to == "C")) {
		return intermediate - 273.15;
	} else if ((from == "Kelvin") || (from == "K")) {
		return intermediate;
	} else {
		throw MeteoException("unknown temperature unit `to'", to);
	}
}

} /* namespace meteo */
