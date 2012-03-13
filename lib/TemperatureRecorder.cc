/*
 * TemperatureRecorder.cc -- implement temperature
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: TemperatureRecorder.cc,v 1.3 2004/02/25 23:48:05 afm Exp $
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
