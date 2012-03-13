/*
 * TemperatureRecorder.cc -- implement temperature
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: TemperatureRecorder.cc,v 1.4 2009/01/10 19:00:25 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
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
