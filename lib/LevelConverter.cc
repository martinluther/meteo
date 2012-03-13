/*
 * LevelConverter.cc -- class to convert values from one system to another
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: LevelConverter.cc,v 1.2 2004/02/25 23:48:05 afm Exp $
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
