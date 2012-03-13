/*
 * BarometerReader.cc -- read temperatures from Davis packets
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: BarometerReader.cc,v 1.4 2004/02/25 23:48:04 afm Exp $
 */
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
