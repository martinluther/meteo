/*
 * OldTemperatureReader.cc -- read temperatures from Davis packets
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#include <OldTemperatureReader.h>

namespace meteo {

OldTemperatureReader::OldTemperatureReader(int o) : NibblePacketReader(o, true, 3) {
}

OldTemperatureReader::~OldTemperatureReader(void) {
}

double	OldTemperatureReader::value(const std::string& packet) const {
	return NibblePacketReader::value(packet)/10.;
}

} /* namespace meteo */
