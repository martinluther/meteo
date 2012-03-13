/*
 * TransmitterRecoder.cc -- encapsulate rain measurements 
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#include <TransmitterRecorder.h>
#include <MeteoException.h>
#include <TransmitterConverter.h>
#include <mdebug.h>

namespace meteo {

TransmitterRecorder::TransmitterRecorder(void) : BasicRecorder("") { }

TransmitterRecorder::TransmitterRecorder(double v) : BasicRecorder("") {
	setValue(v);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "initialized status from %.0f", v);
}

TransmitterRecorder::TransmitterRecorder(double v, const std::string& u) : BasicRecorder(u) {
	setValue(v);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "initialized status from %.0f%s",
		v, u.c_str());
}

TransmitterRecorder::TransmitterRecorder(const std::string& u)
	: BasicRecorder(u) {
}

void	TransmitterRecorder::update(const Value& v) {
	value = v.getValue();
	setValid(true);
	lastupdate.now();
}

} /* namespace meteo */
