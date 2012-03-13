/*
 * LevelConverter.cc -- class to convert values from one system to another
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#include <LevelConverter.h>
#include <MeteoException.h>
#include <mdebug.h>

namespace meteo {

double	LevelConverter::operator()(const double value, const std::string& from) const {
	if (from == getUnit())
		return value;

	if (from != "m")
		throw MeteoException("unknown level unit `from'", from);
	if (getUnit() != "m")
		throw MeteoException("unknown level unit `to'", getUnit());
	return value;
}

} /* namespace meteo */
