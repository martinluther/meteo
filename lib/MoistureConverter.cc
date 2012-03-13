/*
 * MoistureConverter.cc -- class to convert values from one system to another
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: MoistureConverter.cc,v 1.2 2004/02/25 23:48:05 afm Exp $
 */
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
