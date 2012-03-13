/*
 * ShortPacketReader.cc -- implementation of byte reading packets
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: ShortPacketReader.cc,v 1.5 2009/01/10 19:00:25 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <ShortPacketReader.h>
#include <mdebug.h>
#include <MeteoException.h>

namespace meteo {

// construction and destruction
ShortPacketReader::ShortPacketReader(int o, bool s, bool l)
	: BasicPacketReader(o), sign(s), littleendian(l) {
}

ShortPacketReader::~ShortPacketReader(void) {
}

// read a byte from the 
double	ShortPacketReader::value(const std::string& packet) const {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "reading short from packet");
	// make sure data is valid (this may create redundant calls, but this
	// is still better than returning junk to the application
	if (!valid(packet))
		throw MeteoException("trying to read invalid data", "");

	// read the two bytes from the packet
	unsigned char	c1 = (unsigned char)packet[offset];
	unsigned char	c2 = (unsigned char)packet[offset + 1];

	// if byte order is bigendian, we swap the two bytes
	if (!littleendian) {
		unsigned char	temporary = c1;
		c1 = c2; c2 = temporary;
	}

	// at this point c1 is always the least significant byte

	// convert the byte to an unsigned short
	unsigned short	us = (c2 << 8) | c1;
	if (sign) {
		// for signed data, convert to a signed short, which will later
		// be converted to a double
		signed short	s = (signed short)us;
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "reading signed short %hd", s);
		return (double)s;
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "reading unsigned short %hu", us);
	return (double)us;
}

bool	ShortPacketReader::valid(const std::string& packet) const {
	unsigned char	c1 = (unsigned char)packet[offset];
	unsigned char	c2 = (unsigned char)packet[offset + 1];

	// if byte order is bigendian, we swap the two bytes
	if (!littleendian) {
		unsigned char	temporary = c1;
		c1 = c2; c2 = temporary;
	}

	// at this point, c1 is always the least significant byte

	// check validity of both bytes
	unsigned short	us = (c2 << 8) | c1;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "checking validity of %hu", us);
	if (sign) {
		return ((us != 0x7fff) && (us != 0x8000));
	}
	return (us != 0xffff);
}

} /* namespace meteo */
