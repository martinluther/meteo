/*
 * DavisStation.cc -- Davis weather station common functionality
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: DavisStation.cc,v 1.6 2009/01/10 19:32:20 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif /* HAVE_STDLIB_H */
#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif /* HAVE_STDIO_H */
#ifdef HAVE_STRING_H
#include <string.h>
#endif /* HAVE_STRING_H */
#include <DavisStation.h>
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif /* HAVE_ARPA_INET_H */
#include <MeteoException.h>
#include <mdebug.h>
#include <crc.h>

namespace meteo {

// starting the Davis station loop is essentially waiting for an ACK
void	DavisStation::startLoop(int p) {
	// wait for a an ack on the channel
	const char	*expect = "\r\nOK\n\r";
	unsigned int	idx = 0;
	bool	ackfound = false;
	do {
		char	c = getChannel()->recvChar();
		if (c < 0)
			throw MeteoException("failed to read ACK", "");
		if (c == ACK)
			ackfound = true;
		if (c == expect[idx])
			idx++;
		if (idx == strlen(expect))
			ackfound = true;
	} while (!ackfound);

	// remember the number of packets to read
	Station::startLoop(p);
}

std::string	DavisStation::readPacket(void) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
		"waiting for Davis packet (%d bytes)", packetsize);
	std::string	packet = getChannel()->recvString(packetsize);
	// verify the header
	if (packet[0] != 0x01) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"Davis packet does not start with 0x01");
		throw MeteoException("Davis packet doesn't start wih 0x01", "");
	}
	// verify the check sum
	crc_t	crc = (unsigned char)packet[packetsize - 1] |
		((unsigned char)packet[packetsize - 2] << 8);
	if (0 != crc_check(&crc, (unsigned char *)packet.substr(1,
		packetsize - 3).c_str(), packetsize - 3)) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "wrong CRC on packet");
		throw MeteoException("wrong CRC on packet", "");
	}

	// return the packet
	return packet;
}

} /* namespace meteo */
