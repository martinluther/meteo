/*
 * OldTemperatureReader.cc -- read temperatures from Davis packets
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: OldTemperatureReader.cc,v 1.3 2004/02/25 23:48:05 afm Exp $
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
