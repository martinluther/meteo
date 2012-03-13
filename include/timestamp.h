/*
 * timestamp.h -- operations with timestamps
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: timestamp.h,v 1.1 2002/01/18 23:34:27 afm Exp $
 */
#ifndef _TIMESTAMP_H
#define _TIMESTAMP_H

#include <time.h>

extern time_t	localtime2time(const char *ts);
extern time_t	gmtime2time(const char *ts);

#endif /* _TIMESTAMP_H */
