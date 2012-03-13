/*
 * HumidityRecorder.cc -- humidity implementation
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: HumidityRecorder.cc,v 1.2 2004/02/25 23:48:05 afm Exp $
 */
#include <HumidityRecorder.h>
#include <MeteoException.h>
#include <mdebug.h>

namespace meteo {

HumidityRecorder::HumidityRecorder(void) : MinmaxRecorder("%") {
}
HumidityRecorder::HumidityRecorder(double v) : MinmaxRecorder("%") {
	setValue(v);
}
HumidityRecorder::HumidityRecorder(double v, const std::string& u)
	: MinmaxRecorder("%") {
	setValue(v);
}
HumidityRecorder::HumidityRecorder(const std::string& u) : MinmaxRecorder(u) {
}

} /* namespace meteo */
