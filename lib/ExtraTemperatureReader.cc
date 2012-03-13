/*
 * ExtraTemperatureReader.cc -- read extra temperatures from Davis packets
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: ExtraTemperatureReader.cc,v 1.4 2004/02/25 23:48:05 afm Exp $
 */
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
