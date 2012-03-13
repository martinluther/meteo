/*
 * OldTemperatureReader.cc -- read temperatures from Davis packets
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: OldTemperatureReader.cc,v 1.4 2009/01/10 19:00:24 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
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
