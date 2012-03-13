/*
 * PressureRecorder.cc -- implement pressure
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: PressureRecorder.cc,v 1.3 2009/01/10 19:00:24 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
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
