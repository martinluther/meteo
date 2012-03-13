/*
 * NibblePacketReader.cc -- implementation of byte reading packets
 *
 * note that this class works completely little endian, as it is designed
 * for (little endian) Davis Stations
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#include <NibblePacketReader.h>
#include <mdebug.h>
#include <MeteoException.h>

namespace meteo {

// construction and destruction
NibblePacketReader::NibblePacketReader(int o, bool s, int n)
	: BasicPacketReader(o), sign(s), nibbles(n) {
}

NibblePacketReader::~NibblePacketReader(void) {
}

// retrieve a nibble: the index of the nibble starts at the least significant
//                    nibble, i.e. the low nibble of the byte pointed to by
//                    the offset.
unsigned int	NibblePacketReader::getNibble(int i,
			const std::string& packet) const {
	// retrieve the right byte from the packet
	unsigned char	b = packet[i / 2];
	if (i % 2) {
		// odd nibble index, most significant nibble within a byte
		return b >> 4;
	} else {
		// even nibble index, least significant nibble within a byte
		return b & 0x0f;
	}
}

// convert the sequence of nibbles into a unsigned int
unsigned int	NibblePacketReader::rawvalue(const std::string& packet) const {
	unsigned int	result;
	for (int i = 0; i < nibbles; i++) {
		result = (result << 4) + getNibble(i, packet);
	}
	return result;
}

// read a byte from the 
double	NibblePacketReader::value(const std::string& packet) const {
	// make sure data is valid (this may create redundant calls, but this
	// is still better than returning junk to the application
	if (!valid(packet))
		throw MeteoException("trying to read invalid data", "");

	unsigned int	c = rawvalue(packet);
	if (sign) {
		// compute sign extension
		int	cs = c;
		if (c & (0x80000000 >> (32 - nibbles * 4))) {
			cs |= 0xffffffff << (nibbles * 4);
		}
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "reading signed data %d", cs);
		return (double)cs;
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "reading unsigned data %u", c);
	return (double)c;
}

bool	NibblePacketReader::valid(const std::string& packet) const {
	unsigned int	invalidunsigned = 0xffffffff >> (32 - nibbles * 4);
	unsigned int	invalidsigned = 0x7fffffff >> (32 - nibbles * 4);
	unsigned int	c = rawvalue(packet);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "checking validity of %lu", c);
	if (sign) {
		return ((c != invalidunsigned) && (c != invalidsigned));
	}
	return (c != invalidsigned);
}

} /* namespace meteo */
