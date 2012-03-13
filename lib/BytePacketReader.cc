/*
 * BytePacketReader.cc -- implementation of byte reading packets
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: BytePacketReader.cc,v 1.4 2009/01/10 19:00:23 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <BytePacketReader.h>
#include <mdebug.h>
#include <MeteoException.h>

namespace meteo {

// construction and destruction
BytePacketReader::BytePacketReader(int o, bool s)
	: BasicPacketReader(o), sign(s) {
}

BytePacketReader::~BytePacketReader(void) {
}

// read a byte from the 
double	BytePacketReader::value(const std::string& packet) const {
	// make sure data is valid (this may create redundant calls, but this
	// is still better than returning junk to the application
	if (!valid(packet))
		throw MeteoException("trying to read invalid data", "");
	if (sign) {
		char	c = packet[offset];
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "reading signed byte %d",
			(int)c);
		return (double)c;
	}
	unsigned char	c = packet[offset];
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "reading signed byte %u",
		(unsigned int)c);
	return (double)c;
}

bool	BytePacketReader::valid(const std::string& packet) const {
	unsigned char	c = packet[offset];
	if (sign) {
		return ((c != 0x7f) && (c != 0x80));
	}
	return (c != 0xff);
}

} /* namespace meteo */
