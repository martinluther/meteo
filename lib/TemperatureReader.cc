/*
 * TemperatureReader.cc -- read temperatures from Davis packets
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: TemperatureReader.cc,v 1.5 2009/01/10 19:00:25 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
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
