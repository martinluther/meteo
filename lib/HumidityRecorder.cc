/*
 * HumidityRecorder.cc -- humidity implementation
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: HumidityRecorder.cc,v 1.3 2009/01/10 19:00:24 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
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
