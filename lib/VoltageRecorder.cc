/*
 * VoltageRecoder.cc -- encapsulate rain measurements 
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: VoltageRecorder.cc,v 1.3 2009/01/10 19:00:25 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <VoltageRecorder.h>
#include <MeteoException.h>
#include <VoltageConverter.h>
#include <mdebug.h>

namespace meteo {

VoltageRecorder::VoltageRecorder(void) : BasicRecorder("V") { }

VoltageRecorder::VoltageRecorder(double v) : BasicRecorder("V") {
	setValue(v);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "initialized voltage from %.2fV", v);
}

VoltageRecorder::VoltageRecorder(double v, const std::string& u) : BasicRecorder(u) {
	setValue(v);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "initialized voltage from %.2f%s",
		v, u.c_str());
}

VoltageRecorder::VoltageRecorder(const std::string& u) : BasicRecorder(u) {
}

} /* namespace meteo */
