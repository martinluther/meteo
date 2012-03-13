/*
 * TimeRecorder.cc -- water level (no known sensors for this)
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: TimeRecorder.cc,v 1.1 2004/02/26 14:00:19 afm Exp $
 */
#include <TimeRecorder.h>
#include <TimeValue.h>
#include <MeteoException.h>
#include <TimeConverter.h>
#include <mdebug.h>

namespace meteo {

TimeRecorder::TimeRecorder(void) : LastRecorder("unix") {
}

TimeRecorder::TimeRecorder(double v) : LastRecorder("unix") {
	setValue(v);
}

TimeRecorder::TimeRecorder(double v, const std::string& u)
	: LastRecorder(u) {
	setValue(v);
}

TimeRecorder::TimeRecorder(const std::string& u) : LastRecorder(u) {
}

} /* namespace meteo */
