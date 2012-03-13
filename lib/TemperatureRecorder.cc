/*
 * TemperatureRecorder.cc -- implement temperature
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#include <TemperatureRecorder.h>
#include <MeteoException.h>
#include <TemperatureConverter.h>
#include <mdebug.h>

namespace meteo {

// TemperatureRecorder methods
TemperatureRecorder::TemperatureRecorder(void) : MinmaxRecorder("C") {
}
TemperatureRecorder::TemperatureRecorder(double v) : MinmaxRecorder("C") {
	setValue(v);
}

TemperatureRecorder::TemperatureRecorder(double v, const std::string& u)
	: MinmaxRecorder(u) {
	setValue(v);
}

TemperatureRecorder::TemperatureRecorder(const std::string& u) : MinmaxRecorder(u) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "creating temperature recorder %s",
		u.c_str());
}

} /* namespace meteo */
