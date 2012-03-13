/*
 * PressureConverter.cc -- class to convert values from one system to another
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: PressureConverter.cc,v 1.3 2009/01/10 19:00:24 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <PressureConverter.h>
#include <MeteoException.h>
#include <mdebug.h>

namespace meteo {

double	PressureConverter::operator()(const double value, const std::string& from) const {
	if (from == getUnit())
		return value;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "converting %.2f from %s to %s",
		value, from.c_str(), getUnit().c_str());

	// convert to intermediate value
	double	intermediate;
	if (from == "hPa") {
		intermediate = value;
	} else if (from == "mmHg") {
		intermediate = 1.333224 * value;
	} else if (from == "inHg") {
		intermediate = 33.8639 * value;
	} else {
		throw MeteoException("unknown Pressure unit `from'", from);
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "hPa pressure: %.2f", intermediate);

	// convert intermediate value to target unit
	const std::string&	to = getUnit();
	if (to == "hPa") {
		return intermediate;
	} else if (to == "mmHg") {
		return intermediate / 1.333224;
	} else if (to == "inHg") {
		return intermediate * 0.03970079 / 1.333224;
	} else {
		throw MeteoException("unknown Pressure unit `to'", to);
	}
}

} /* namespace meteo */
