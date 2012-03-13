/*
 * MoistureConverter.cc -- class to convert values from one system to another
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: MoistureConverter.cc,v 1.3 2009/01/10 19:00:24 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <MoistureConverter.h>
#include <MeteoException.h>
#include <mdebug.h>

namespace meteo {

double	MoistureConverter::operator()(const double value, const std::string& from) const {
	if (from == getUnit())
		return value;

	if (from != "cb")
		throw MeteoException("unknown moisture unit `from'", from);
	if (getUnit() != "cb")
		throw MeteoException("unknown moisture unit `to'", getUnit());
	return value;
}

} /* namespace meteo */
