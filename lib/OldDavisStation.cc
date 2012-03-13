/*
 * OldDavisStation.cc -- Old Davis weather station common functionality
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: OldDavisStation.cc,v 1.9 2004/02/25 23:48:05 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <OldDavisStation.h>
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#include <MeteoException.h>
#include <mdebug.h>
#include <ChannelFactory.h>

namespace meteo {

OldDavisStation::OldDavisStation(const std::string& n, int ps)
	: DavisStation(n, ps) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "OldDavisStation::OldDavisStation(%s, "
		"%d)", n.c_str(), ps);
}

// reading bytes from old style stations is always almost identical, note
// that this method reads data from the station memory (not link memory)
std::string	OldDavisStation::readBytes(int bank, int offs, int count) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "readBytes(%d, %x, %d)",
		bank, offs, count);
	// retrieve the rain calibration number from the station
	unsigned char	cmd[6];
	cmd[0] = 'W'; cmd[1] = 'R'; cmd[2] = 'D';
	cmd[3] = 0x4 | (count << 5) ; cmd[4] = offs; cmd[5] = 0xd;
	std::string	cmdstr((char *)cmd, 6);

	// send the command through the cannel
	getChannel()->sendString(cmdstr);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "sent WRD command");
	std::string	reply = getChannel()->recvString(count + 1);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "got %d bytes reply to WRD",
		reply.length());

	// wait for an ACK reply
	if (ACK != reply[0]) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "cannot read CAL number at %d",
			offs);
		throw MeteoException("cannot read CAL number", "");
	}

	// the reply is already a string
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "sending %d bytes back", reply.size());
	return reply;
}

// Old style Davis stations all use the same loop command syntax, this
// changes only with the Vantage Pro
void	OldDavisStation::startLoop(int p) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
		"requesting %d packets from old Davis station", p);
	unsigned short	n = 65536 - p;
	unsigned char	b1 = n & 0xff;
	unsigned char	b2 = (n & 0xff00) >> 8;
	unsigned char	el = 0xd;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "sending command LOOP %02x %02x \\r",
		b1, b2);

	std::string	cmd = std::string("LOOP")
				+ std::string((char *)&b1, 1)
				+ std::string((char *)&b2, 1)
				+ std::string((char *)&el, 1);
	
	// format the command as a hex string for easier debugging
	if (debug) {
		std::string	scmd;
		char		b[8];
		for (unsigned int i = 0; i < cmd.size(); i++) {
			snprintf(b, sizeof(b), "%02x ", (unsigned char)cmd[i]);
			scmd += b;
		}
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"(old) station LOOP command; %s", scmd.c_str());
	}
	getChannel()->sendString(cmd);

	// now wait for ack, that's identical for all Davis stations
	DavisStation::startLoop(p);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "OldDavisStation::startLoop complete");
}

} /* namespace meteo */

