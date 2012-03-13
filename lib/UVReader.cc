/*
 * UVReader.cc -- read uv index from Davis packets
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
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
