/*
 * timestamp.c -- operations with timestamps
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: timestamp.c,v 1.2 2001/12/26 22:10:47 afm Exp $
 */
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <timestamp.h>

extern int	optind;
extern char	*optarg;

time_t	string2time(const char *ts) {
	char		*wc;
	struct tm	tt;
	int		l;

	/* timestamp must be complete					*/
	if ((l = strlen(ts)) < 12)
		return -1;

	/* create a local working copy					*/
	wc = (char *)alloca(l + 1);
	memcpy(wc, ts, l + 1);

	tt.tm_sec = 0; wc[12] = '\0';
	tt.tm_min = atoi(&wc[10]); wc[10] = '\0';
	tt.tm_hour = atoi(&wc[8]); wc[8] = '\0';
	tt.tm_mday = atoi(&wc[6]); wc[6] = '\0';
	tt.tm_mon = atoi(&wc[4]) - 1; wc[4] = '\0';
	tt.tm_year = atoi(wc) - 1900;
	tt.tm_yday = tt.tm_wday = tt.tm_isdst = 0;

	/* make time from that						*/
	return mktime(&tt);
}
