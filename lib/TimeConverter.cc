/*
 * TimeConverter.cc -- class to convert values from one system to another
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: TimeConverter.cc,v 1.1 2004/02/26 14:00:19 afm Exp $
 */
#include <TimeConverter.h>
#include <MeteoException.h>
#include <mdebug.h>

namespace meteo {

double	TimeConverter::operator()(const double value, const std::string& from) const {
	if (from == getUnit())
		return value;

	if (from != "unix")
		throw MeteoException("unknown level unit `from'", from);
	if (getUnit() != "unix")
		throw MeteoException("unknown level unit `to'", getUnit());
	return value;
}

} /* namespace meteo */
