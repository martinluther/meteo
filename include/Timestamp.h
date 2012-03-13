/*
 * Timestamp.h -- operations with timestamps
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: Timestamp.h,v 1.4 2003/10/10 22:29:01 afm Exp $
 */
#ifndef _Timestamp_H
#define _Timestamp_H

#include <time.h>
#include <string>

namespace meteo {

class Timestamp {
	time_t	t;
public:
	Timestamp(const std::string& ts);
	~Timestamp(void) { }
	time_t	getTime(void) const { return t; }
};

} /* namespace meteo */

#endif /* _Timestamp_H */
