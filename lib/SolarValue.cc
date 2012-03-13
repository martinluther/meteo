/*
 * SolarValue.cc -- implement solar radiation
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#include <SolarValue.h>
#include <MeteoException.h>
#include <SolarConverter.h>
#include <mdebug.h>

namespace meteo {

SolarValue::SolarValue(void) : BasicValue("W/m2") {
}

SolarValue::SolarValue(double v) : BasicValue("W/m2") {
	setValue(v);
}
SolarValue::SolarValue(double v, const std::string& u) : BasicValue(u) {
	setValue(v);
}
SolarValue::SolarValue(const std::string& u) : BasicValue(u) {
}
void	SolarValue::setUnit(const std::string& targetunit) {
	if (hasValue()) {
		SolarConverter(targetunit).convert(this);
	}
}

} /* namespace meteo */
