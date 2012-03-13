/*
 * PressureValue.cc -- implement pressure
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: PressureValue.cc,v 1.2 2004/02/25 23:48:05 afm Exp $
 */
#include <PressureValue.h>
#include <PressureConverter.h>
#include <MeteoException.h>
#include <Converter.h>
#include <mdebug.h>

namespace meteo {

PressureValue::PressureValue(void) : BasicValue("hPa") {
}
PressureValue::PressureValue(double v) : BasicValue("hPa") {
	setValue(v);
}
PressureValue::PressureValue(double v, const std::string& u) : BasicValue(u) {
	setValue(v);
}
PressureValue::PressureValue(const std::string& u) : BasicValue(u) {
}
void	PressureValue::setUnit(const std::string& targetunit) {
	if (hasValue()) {
		PressureConverter(targetunit).convert(this);
	}
}

} /* namespace meteo */
