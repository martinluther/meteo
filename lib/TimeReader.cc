/*
 * TimeReader.cc -- read a time value off a Vantage Pro loop packet
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: TimeReader.cc,v 1.4 2009/01/10 19:00:25 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <TimeReader.h>
#ifdef HAVE_TIME_H
#include <time.h>
#endif /* HAVE_TIME_H */

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
