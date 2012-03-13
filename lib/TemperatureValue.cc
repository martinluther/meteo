/*
 * TemperatureValue.cc -- implement temperature
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
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
