/*
 * VoltageReader.cc -- read temperatures from Davis packets
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: VoltageReader.cc,v 1.4 2009/01/10 19:00:25 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <VoltageReader.h>

namespace meteo {

VoltageReader::VoltageReader(int o)
	: ShortPacketReader(o, false, true) {
}

VoltageReader::~VoltageReader(void) {
}

double	VoltageReader::value(const std::string& packet) const {
	return (3. * ShortPacketReader::value(packet))/512.;
}

} /* namespace meteo */
