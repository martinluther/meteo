/*
 * ExtraTemperatureReader.cc -- read extra temperatures from Davis packets
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
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
