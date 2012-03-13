/*
 * UVConverter.cc -- class to convert values from one system to another
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: UVConverter.cc,v 1.3 2004/02/25 23:48:06 afm Exp $
 */
#include <UVConverter.h>
#include <MeteoException.h>
#include <mdebug.h>

namespace meteo {

double	UVConverter::operator()(const double value, const std::string& from) const {
	// don't do anything if the two units coincide
	if (from == getUnit())
		return value;

	// inform what we will be doing
	const std::string&	to = getUnit();
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "converting %.2f from %s to %s",
		value, from.c_str(), getUnit().c_str());

	// convert to intermediate value
	double	intermediate;
	if (from == "med") {
		intermediate = (7./3.) * value;
	} else if (from == "index") {
		intermediate = value;
	} else {
		throw MeteoException("unknown uv unit `from'", from);
	}
	
	// convert from intermediate to to
	double	result;
	if (to == "med") {
		result = (3./7.) * intermediate;
	} else if (to == "index") {
		result = intermediate;
	} else {
		throw MeteoException("unknown uv unit `to'", getUnit());
	}
	return result;
}

} /* namespace meteo */
