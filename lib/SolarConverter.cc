/*
 * SolarConverter.cc -- class to convert values from one system to another
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#include <SolarConverter.h>
#include <MeteoException.h>
#include <mdebug.h>

namespace meteo {

double	SolarConverter::operator()(const double value, const std::string& from) const {
	if (from == getUnit())
		return value;

	if (from != "W/m2")
		throw MeteoException("unknown solar radiation unit `from'", from);
	if (getUnit() != "W/m2")
		throw MeteoException("unknown solar radiation unit `to'", getUnit());
	return value;
}

} /* namespace meteo */
