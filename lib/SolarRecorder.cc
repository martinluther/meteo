/*
 * SolarRecorder.cc -- implement solar radiation
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
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