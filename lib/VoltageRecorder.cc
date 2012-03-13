/*
 * VoltageRecoder.cc -- encapsulate rain measurements 
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: VoltageRecorder.cc,v 1.2 2004/02/25 23:48:06 afm Exp $
 */
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
