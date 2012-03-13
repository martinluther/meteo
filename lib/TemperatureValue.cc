/*
 * TemperatureValue.cc -- implement temperature
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: TemperatureValue.cc,v 1.3 2009/01/10 19:00:25 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <TemperatureValue.h>
#include <MeteoException.h>
#include <TemperatureConverter.h>
#include <mdebug.h>

namespace meteo {

// TemperatureValue methods
TemperatureValue::TemperatureValue(void) : BasicValue("C") {
}
TemperatureValue::TemperatureValue(double v) : BasicValue("C") {
	setValue(v);
}

TemperatureValue::TemperatureValue(double v, const std::string& u)
	: BasicValue(u) {
	setValue(v);
}

TemperatureValue::TemperatureValue(const std::string& u) : BasicValue(u) {
}

void	TemperatureValue::setUnit(const std::string& targetunit) {
	if (hasValue()) {
		TemperatureConverter(targetunit).convert(this);
	}
}

} /* namespace meteo */
