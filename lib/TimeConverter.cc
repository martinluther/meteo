/*
 * TimeConverter.cc -- class to convert values from one system to another
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: TimeConverter.cc,v 1.2 2009/01/10 19:00:25 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
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
