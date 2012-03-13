/*
 * RainRateRecorder.cc -- implement rain rate recording
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#include <RainRateRecorder.h>
#include <MeteoException.h>
#include <RainRateConverter.h>
#include <mdebug.h>

namespace meteo {

// RainRateRecorder methods
RainRateRecorder::RainRateRecorder(void) : MinmaxRecorder("mm/h") {
}
RainRateRecorder::RainRateRecorder(double v) : MinmaxRecorder("mm/h") {
	setValue(v);
}

RainRateRecorder::RainRateRecorder(double v, const std::string& u)
	: MinmaxRecorder(u) {
	setValue(v);
}

RainRateRecorder::RainRateRecorder(const std::string& u) : MinmaxRecorder(u) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "creating rain rate recorder %s",
		u.c_str());
}

} /* namespace meteo */
