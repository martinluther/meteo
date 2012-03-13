/*
 * MeteoValue.cc -- handle meteo value including maximum and minimum and
 *                  aggregation
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#include <MeteoValue.h>
#include <MeteoException.h>
#include <Converter.h>
#include <mdebug.h>

namespace meteo {

// constructors and destructors
MeteoValue::MeteoValue(void) {
	max = -1000000.;
	min =  1000000.;
	start.now();
	hasvalue = false;
}
MeteoValue::~MeteoValue(void) {
}

// update the value
void	MeteoValue::update(double v) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "updating with value %.2f", v);
	if (hasvalue) {
		// compute new average
		double	t1 = (lastupdate - start).getValue();
		Timeval	s;
		s.now();
		double	t2 = (s - lastupdate).getValue();
		value = (t1 * value + t2 * v)/(t1 + t2);
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "new average: %.2f", value);
		lastupdate.now();

		// update the maximum
		if (v > max)
			max = v;

		// update the minimum
		if (v < min)
			min = v;
	} else {
		min = max = value = v;
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "new value: %.2f", value);
		lastupdate.now();
		hasvalue = true;
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "update complete");
}

// set the value, this resets time stamps and max/min
void	MeteoValue::setValue(double v) {
	start.now();
	lastupdate.now();
	value = v;
	max = v;
	min = v;
	hasvalue = true;
}

// retrieve string versions
std::string	MeteoValue::getValueString(void) const {
	char	buffer[32];
	if (!hasvalue)
		return std::string("NULL");
	snprintf(buffer, sizeof(buffer), "%.2f", value);
	return std::string(buffer);
}

std::string	MeteoValue::getMinString(void) const {
	char	buffer[32];
	if (!hasvalue)
		return std::string("NULL");
	snprintf(buffer, sizeof(buffer), "%.2f", min);
	return std::string(buffer);
}

std::string	MeteoValue::getMaxString(void) const {
	char	buffer[32];
	if (!hasvalue)
		return std::string("NULL");
	snprintf(buffer, sizeof(buffer), "%.2f", max);
	return std::string(buffer);
}

double	MeteoValue::getMax(void) const {
	if (!hasvalue)
		throw MeteoException("no max value", "");
	return value;
}

double	MeteoValue::getMin(void) const {
	if (!hasvalue)
		throw MeteoException("no min value", "");
	return min;
}

double	MeteoValue::getValue(void) const {
	if (!hasvalue)
		throw MeteoException("no avg value", "");
	return max;
}

void	MeteoValue::reset(void) {
	max = -1000000.;
	min =  1000000.;
	hasvalue = false;
	start.now();
}

// TemperatureValue methods
TemperatureValue::TemperatureValue(double v) {
	setValue(v); setUnit("C");
}
TemperatureValue::TemperatureValue(double v, const std::string& u) {
	setValue(v); setUnit(u);
}
TemperatureValue::TemperatureValue(const std::string& u) {
	setUnit(u);
}
void	TemperatureValue::update(const TemperatureValue& t) {
	if (!t.hasValue())
		return;
	double	tt = TemperatureConverter(getUnit())(t).getValue();
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "temp update value: %.2f", tt);
	MeteoValue::update(tt);
}

// HumidityValue methods
HumidityValue::HumidityValue(double v) {
	setValue(v); setUnit("%");
}
HumidityValue::HumidityValue(double v, const std::string& u) {
	setValue(v); setUnit(u);
}
HumidityValue::HumidityValue(const std::string& u) {
	setUnit(u);
}
void	HumidityValue::update(const HumidityValue& t) {
	if (!t.hasValue()) return;
	MeteoValue::update(HumidityConverter(getUnit())(t).getValue());
}
TemperatureValue	HumidityValue::Dewpoint(const TemperatureValue& t) {
	double	tempC = TemperatureConverter("C")(t).getValue();
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
		return TemperatureConverter(t.getUnit())(TemperatureValue(dp, "C"));
	}
}

// PressureValue methods
PressureValue::PressureValue(double v) {
	setValue(v); setUnit("hPa");
}
PressureValue::PressureValue(double v, const std::string& u) {
	setValue(v); setUnit(u);
}
PressureValue::PressureValue(const std::string& u) {
	setUnit(u);
}
void	PressureValue::update(const PressureValue& t) {
	if (!t.hasValue()) return;
	MeteoValue::update(PressureConverter(getUnit())(t).getValue());
}

// SolarValue methods
SolarValue::SolarValue(double v) {
	setValue(v); setUnit("W/m2");
}
SolarValue::SolarValue(double v, const std::string& u) {
	setValue(v); setUnit(u);
}
SolarValue::SolarValue(const std::string& u) {
	setUnit(u);
}
void	SolarValue::update(const SolarValue& t) {
	if (!t.hasValue()) return;
	MeteoValue::update(SolarConverter(getUnit())(t).getValue());
}

// UVValue methods
UVValue::UVValue(double v) {
	setValue(v); setUnit("index");
}
UVValue::UVValue(double v, const std::string& u) {
	setValue(v); setUnit(u);
}
UVValue::UVValue(const std::string& u) {
	setUnit(u);
}
void	UVValue::update(const UVValue& t) {
	if (!t.hasValue()) return;
	MeteoValue::update(UVConverter(getUnit())(t).getValue());
}

} /* meteo */
