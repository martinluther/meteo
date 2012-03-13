/*
 * PressureRecorder.cc -- implement pressure
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: PressureRecorder.cc,v 1.2 2004/02/25 23:48:05 afm Exp $
 */
#include <PressureRecorder.h>
#include <MeteoException.h>
#include <PressureConverter.h>
#include <mdebug.h>

namespace meteo {

PressureRecorder::PressureRecorder(void) : MinmaxRecorder("hPa") {
}
PressureRecorder::PressureRecorder(double v) : MinmaxRecorder("hPa") {
	setValue(v);
}
PressureRecorder::PressureRecorder(double v, const std::string& u)
	: MinmaxRecorder(u) {
	setValue(v);
}
PressureRecorder::PressureRecorder(const std::string& u) : MinmaxRecorder(u) {
}

} /* namespace meteo */
