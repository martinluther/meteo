/*
 * TimeReader.cc -- read a time value off a Vantage Pro loop packet
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#include <TimeReader.h>
#include <time.h>

namespace meteo {

TimeReader::TimeReader(int o) : ShortPacketReader(o, false, true) {
}

TimeReader::~TimeReader(void) {
}

double	TimeReader::value(const std::string& packet) const {
	unsigned char	h = (unsigned char)packet[offset];
	unsigned char	m = (unsigned char)packet[offset + 1];

	// compute broken down time of current time
	time_t		now = time(NULL);
	struct tm	*nowtm = localtime(&now);

	// replace hour and minute
	nowtm->tm_sec = 0;
	nowtm->tm_min = m;
	nowtm->tm_hour = h;

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
