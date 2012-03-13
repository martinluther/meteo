/*
 * RainReader.cc -- read rain from Davis packets
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#include <RainReader.h>

namespace meteo {

RainReader::RainReader(int o)
	: ShortPacketReader(o, false, true) { // unsigned, littleendian
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "rain reader created at %p", this);
}

RainReader::~RainReader(void) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "rain reader at %p destroyed", this);
}

// this value method assumes a 0.01 in rain collector, if a different rain
// collector is used, calibration will fix the value
double	RainReader::value(const std::string& packet) const {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "reading Rain");
	double 	f = ShortPacketReader::value(packet)/100.;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "rain value: %f", f);
	return f;
}

} /* namespace meteo */
