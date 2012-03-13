/*
 * HumidityConverter.cc -- class to convert values from one system to another
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: HumidityConverter.cc,v 1.3 2009/01/10 19:00:24 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <HumidityConverter.h>
#include <MeteoException.h>
#include <mdebug.h>

namespace meteo {

double	HumidityConverter::operator()(const double value, const std::string& from) const {
	if (from == getUnit())
		return value;

	if (from != "%")
		throw MeteoException("unknown Humidity unit `from'", from);
	if (getUnit() != "%")
		throw MeteoException("unknown Humidity unit `to'", getUnit());
	return value;
}

} /* namespace meteo */
