/*
 * SolarRecorder.cc -- implement solar radiation
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: SolarRecorder.cc,v 1.3 2009/01/10 19:00:25 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <SolarRecorder.h>
#include <MeteoException.h>
#include <SolarConverter.h>
#include <mdebug.h>

namespace meteo {

SolarRecorder::SolarRecorder(void) : MinmaxRecorder("W/m2") {
}
SolarRecorder::SolarRecorder(double v) : MinmaxRecorder("W/m2") {
	setValue(v);
}
SolarRecorder::SolarRecorder(double v, const std::string& u) : MinmaxRecorder(u) {
	setValue(v);
}
SolarRecorder::SolarRecorder(const std::string& u) : MinmaxRecorder(u) {
}

} /* namespace meteo */
