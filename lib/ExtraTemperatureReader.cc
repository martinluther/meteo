/*
 * ExtraTemperatureReader.cc -- read extra temperatures from Davis packets
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: ExtraTemperatureReader.cc,v 1.5 2009/01/10 19:00:24 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <ExtraTemperatureReader.h>

namespace meteo {

ExtraTemperatureReader::ExtraTemperatureReader(int o)
	: BytePacketReader(o, false) {
}

ExtraTemperatureReader::~ExtraTemperatureReader(void) {
}

double	ExtraTemperatureReader::value(const std::string& packet) const {
	return BytePacketReader::value(packet) - 90.;
}

} /* namespace meteo */
