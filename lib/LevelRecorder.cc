/*
 * LevelRecorder.cc -- leaf wetness value
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#include <LevelRecorder.h>
#include <LevelValue.h>
#include <MeteoException.h>
#include <LevelConverter.h>
#include <mdebug.h>

namespace meteo {

LevelRecorder::LevelRecorder(void) : MinmaxRecorder("m") {
}

LevelRecorder::LevelRecorder(double v) : MinmaxRecorder("m") {
	setValue(v);
}

LevelRecorder::LevelRecorder(double v, const std::string& u)
	: MinmaxRecorder(u) {
	setValue(v);
}

LevelRecorder::LevelRecorder(const std::string& u) : MinmaxRecorder(u) {
}

} /* namespace meteo */
