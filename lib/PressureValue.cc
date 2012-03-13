/*
 * PressureValue.cc -- implement pressure
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: PressureValue.cc,v 1.3 2009/01/10 19:00:24 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
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
