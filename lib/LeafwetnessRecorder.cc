/*
 * LeafwetnessRecorder.cc -- leaf wetness value
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#include <LeafwetnessRecorder.h>
#include <LeafwetnessValue.h>
#include <MeteoException.h>
#include <LeafwetnessConverter.h>
#include <mdebug.h>

namespace meteo {

LeafwetnessRecorder::LeafwetnessRecorder(void) : MinmaxRecorder("index") {
}

LeafwetnessRecorder::LeafwetnessRecorder(double v) : MinmaxRecorder("index") {
	setValue(v);
}

LeafwetnessRecorder::LeafwetnessRecorder(double v, const std::string& u)
	: MinmaxRecorder(u) {
	setValue(v);
}

LeafwetnessRecorder::LeafwetnessRecorder(const std::string& u) : MinmaxRecorder(u) {
}

} /* namespace meteo */