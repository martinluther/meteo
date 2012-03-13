/*
 * WindSpeed.cc -- implement temperature
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: WindSpeed.cc,v 1.2 2004/02/25 23:48:06 afm Exp $
 */
#include <WindSpeed.h>
#include <MeteoException.h>
#include <TemperatureConverter.h>
#include <mdebug.h>

namespace meteo {

// WindSpeed methods
WindSpeed::WindSpeed(void) : BasicValue("m/s") {
}
WindSpeed::WindSpeed(double v) : BasicValue("m/s") {
	setValue(v);
}

WindSpeed::WindSpeed(double v, const std::string& u)
	: BasicValue(u) {
	setValue(v);
}

WindSpeed::WindSpeed(const std::string& u) : BasicValue(u) {
}

void	WindSpeed::setUnit(const std::string& targetunit) {
	if (hasValue()) {
		TemperatureConverter(targetunit).convert(this);
	}
}

} /* namespace meteo */
