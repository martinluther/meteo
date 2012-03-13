/*
 * VoltageConverter.cc -- class to convert values from one system to another
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#include <VoltageConverter.h>
#include <MeteoException.h>
#include <mdebug.h>

namespace meteo {

double	VoltageConverter::operator()(const double value, const std::string& from) const {
	if (from == getUnit())
		return value;

	if (from != "V")
		throw MeteoException("unknown Voltage unit `from'", from);
	if (getUnit() != "V")
		throw MeteoException("unknown Voltage unit `to'", getUnit());
	return value;
}

} /* namespace meteo */
