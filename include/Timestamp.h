/*
 * Timestamp.h -- operations with timestamps
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: Timestamp.h,v 1.5 2006/05/16 11:19:54 afm Exp $
 */
#ifndef _Timestamp_H
#define _Timestamp_H

#include <time.h>
#include <string>

namespace meteo {

class Timestamp {
	time_t	t;
public:
	Timestamp();	// now
	Timestamp(const std::string& ts);
	~Timestamp(void) { }
	time_t	getTime(void) const { return t; }
	std::string	ctime() const;
	std::string	strftime(const char *format) const;
};

} /* namespace meteo */

#endif /* _Timestamp_H */
