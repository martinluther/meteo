/*
 * TimeReader.cc -- read a time value off a Vantage Pro loop packet
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: TimeReader.cc,v 1.3 2004/02/26 23:43:12 afm Exp $
 */
#include <TimeReader.h>
#include <time.h>

namespace meteo {

TimeReader::TimeReader(int o) : ShortPacketReader(o, false, true) {
}

TimeReader::~TimeReader(void) {
}

double	TimeReader::value(const std::string& packet) const {
	unsigned short	lowbyte = (unsigned char)packet[offset];
	unsigned short	highbyte = (unsigned char)packet[offset + 1];
	unsigned short	stationtime = (highbyte << 8) | lowbyte;

	// compute broken down time of current time
	time_t		now = time(NULL);
	struct tm	*nowtm = localtime(&now);

	// replace hour and minute
	nowtm->tm_sec = 0;
	nowtm->tm_min = stationtime % 100;
	nowtm->tm_hour = stationtime / 100;

	// convert back to unix time
	return mktime(nowtm);
}

bool	TimeReader::valid(const std::string& packet) const {
	unsigned char	h = (unsigned char)packet[offset];
	unsigned char	m = (unsigned char)packet[offset + 1];

	if ((h > 23) || (m > 59))
		return false;
	return true;
}

} /* namespace meteo */
