/*
 * LeafwetnessConverter.cc -- class to convert values from one system to another
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: LeafwetnessConverter.cc,v 1.2 2004/02/25 23:48:05 afm Exp $
 */
#include <LeafwetnessConverter.h>
#include <MeteoException.h>
#include <mdebug.h>

namespace meteo {

double	LeafwetnessConverter::operator()(const double value, const std::string& from) const {
	if (from == getUnit())
		return value;

	if (from != "")
		throw MeteoException("unknown leaf wetness unit `from'", from);
	if (getUnit() != "")
		throw MeteoException("unknown leaf wetness unit `to'", getUnit());
	return value;
}

} /* namespace meteo */
