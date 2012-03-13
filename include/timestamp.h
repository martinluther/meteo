/*
 * timestamp.h -- operations with timestamps
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: timestamp.h,v 1.3 2003/06/06 15:11:05 afm Exp $
 */
#ifndef _TIMESTAMP_H
#define _TIMESTAMP_H

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

extern time_t	localtime2time(const char *ts);
extern time_t	gmtime2time(const char *ts);

#ifdef __cplusplus
}
#endif

#endif /* _TIMESTAMP_H */
