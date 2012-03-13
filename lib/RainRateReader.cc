/*
 * RainRateReader.cc -- read rain rate from Davis packets
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: RainRateReader.cc,v 1.4 2009/01/10 19:00:24 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
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
