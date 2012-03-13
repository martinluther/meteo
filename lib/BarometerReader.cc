/*
 * BarometerReader.cc -- read temperatures from Davis packets
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: BarometerReader.cc,v 1.5 2009/01/10 19:00:23 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <BarometerReader.h>
#include <mdebug.h>

namespace meteo {

BarometerReader::BarometerReader(int o)
	: ShortPacketReader(o, false, true) { // unsigned, littleendian
}

BarometerReader::~BarometerReader(void) {
}

double	BarometerReader::value(const std::string& packet) const {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "reading barometer");
	double	result = ShortPacketReader::value(packet)/1000.;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "barometer reading: %f", result);
	return result;
}

} /* namespace meteo */
