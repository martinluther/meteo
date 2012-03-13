/*
 * MoistureRecorder.cc -- implement soil moisture value
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: MoistureRecorder.cc,v 1.3 2009/01/10 19:00:24 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <MoistureRecorder.h>
#include <MeteoException.h>
#include <MoistureConverter.h>
#include <mdebug.h>

namespace meteo {

MoistureRecorder::MoistureRecorder(void) : MinmaxRecorder("cb") {
}
MoistureRecorder::MoistureRecorder(double v) : MinmaxRecorder("cb") {
	setValue(v);
}
MoistureRecorder::MoistureRecorder(double v, const std::string& u)
	: MinmaxRecorder(u) {
	setValue(v);
}
MoistureRecorder::MoistureRecorder(const std::string& u) : MinmaxRecorder(u) {
}

} /* namespace meteo */
