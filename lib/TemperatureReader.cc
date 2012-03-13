/*
 * TemperatureReader.cc -- read temperatures from Davis packets
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#include <TemperatureReader.h>
#include <mdebug.h>

namespace meteo {

TemperatureReader::TemperatureReader(int o)
	: ShortPacketReader(o, true, true) {
}

TemperatureReader::~TemperatureReader(void) {
}

double	TemperatureReader::value(const std::string& packet) const {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "reading signed short from offset %d",
		offset);
	return ShortPacketReader::value(packet)/10.;
}

} /* namespace meteo */
