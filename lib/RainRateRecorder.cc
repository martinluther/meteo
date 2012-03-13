/*
 * RainRateRecorder.cc -- implement rain rate recording
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: RainRateRecorder.cc,v 1.4 2009/01/10 19:00:24 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <RainRateRecorder.h>
#include <MeteoException.h>
#include <RainRateConverter.h>
#include <mdebug.h>

namespace meteo {

// RainRateRecorder methods
RainRateRecorder::RainRateRecorder(void) : MinmaxRecorder("mm/h") {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "RainRateRecorder created");
}
RainRateRecorder::RainRateRecorder(double v) : MinmaxRecorder("mm/h") {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "RainRateRecorder created: %f", v);
	setValue(v);
}

RainRateRecorder::RainRateRecorder(double v, const std::string& u)
	: MinmaxRecorder(u) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "RainRateRecorder created: %f (%s)",
		v, u.c_str());
	setValue(v);
}

RainRateRecorder::RainRateRecorder(const std::string& u) : MinmaxRecorder(u) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "creating rain rate recorder %s",
		u.c_str());
}

} /* namespace meteo */
