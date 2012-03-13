/*
 * HumidityValue.cc -- humidity implementation
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: HumidityValue.cc,v 1.2 2004/02/25 23:48:05 afm Exp $
 */
#include <HumidityValue.h>
#include <MeteoException.h>
#include <HumidityConverter.h>
#include <TemperatureConverter.h>
#include <mdebug.h>
#include <math.h>

namespace meteo {

HumidityValue::HumidityValue(void) : BasicValue("%") {
}
HumidityValue::HumidityValue(double v) : BasicValue("%") {
	setValue(v);
}
HumidityValue::HumidityValue(double v, const std::string& u) : BasicValue(u) {
	setValue(v);
}
HumidityValue::HumidityValue(const std::string& u) : BasicValue(u) {
}
void	HumidityValue::setUnit(const std::string& targetunit) {
	if (hasValue()) {
		HumidityConverter(targetunit).convert(this);
	}
}
TemperatureValue	HumidityValue::Dewpoint(const TemperatureValue& t) {
	double	tempC = TemperatureConverter("C")(t.getValue(), t.getUnit());
	double	humidity = getValue();

	double	ews, dp;
	ews = humidity * 0.01 * exp((17.502 * tempC)/(240.9 + tempC));
	if (ews < 0)
		return -273.0;
	dp = 240.9 * log(ews)/(17.5 - log(ews));
	if (isnan(dp)) {
		TemperatureValue	tv;
		tv.setUnit(t.getUnit());
		return tv;
	} else {
		return TemperatureConverter(t.getUnit())(dp, "C");
	}
}

} /* namespace meteo */
