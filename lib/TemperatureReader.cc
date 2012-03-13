/*
 * TemperatureReader.cc -- read temperatures from Davis packets
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: TemperatureReader.cc,v 1.4 2004/02/25 23:48:05 afm Exp $
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
