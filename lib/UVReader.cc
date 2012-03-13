/*
 * UVReader.cc -- read uv index from Davis packets
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: UVReader.cc,v 1.4 2009/01/10 19:00:25 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <UVReader.h>

namespace meteo {

UVReader::UVReader(int o)
	: BytePacketReader(o, false) {
}

UVReader::~UVReader(void) {
}

double	UVReader::value(const std::string& packet) const {
	return BytePacketReader::value(packet)/10.;
}

} /* namespace meteo */
