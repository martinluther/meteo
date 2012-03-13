/*
 * UVRecorder.cc -- implement UV value
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#include <UVRecorder.h>
#include <MeteoException.h>
#include <UVConverter.h>
#include <mdebug.h>

namespace meteo {

UVRecorder::UVRecorder(void) : MinmaxRecorder("index") {
}
UVRecorder::UVRecorder(double v) : MinmaxRecorder("index") {
	setValue(v);
}
UVRecorder::UVRecorder(double v, const std::string& u) : MinmaxRecorder(u) {
	setValue(v);
}
UVRecorder::UVRecorder(const std::string& u) : MinmaxRecorder(u) {
}

} /* namespace meteo */