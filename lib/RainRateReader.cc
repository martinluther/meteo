/*
 * RainRateReader.cc -- read rain rate from Davis packets
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: RainRateReader.cc,v 1.3 2004/02/25 23:48:05 afm Exp $
 */
#include <RainRateReader.h>

namespace meteo {

RainRateReader::RainRateReader(int o) : BytePacketReader(o, false) {
}

RainRateReader::~RainRateReader(void) {
}

double	RainRateReader::value(const std::string& packet) const {
	return BytePacketReader::value(packet)/10.;
}

} /* namespace meteo */
