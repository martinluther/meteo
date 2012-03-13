/*
 * VoltageReader.cc -- read temperatures from Davis packets
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#include <VoltageReader.h>

namespace meteo {

VoltageReader::VoltageReader(int o)
	: ShortPacketReader(o, false, true) {
}

VoltageReader::~VoltageReader(void) {
}

double	VoltageReader::value(const std::string& packet) const {
	return (3. * ShortPacketReader::value(packet))/512.;
}

} /* namespace meteo */
