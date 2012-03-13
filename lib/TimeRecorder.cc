/*
 * TimeRecorder.cc -- water level (no known sensors for this)
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: TimeRecorder.cc,v 1.2 2009/01/10 19:00:25 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
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
