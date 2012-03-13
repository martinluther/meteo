/*
 * timestamp.c -- operations with timestamps
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: timestamp.c,v 1.5 2003/08/01 08:30:17 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <time.h>
#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <timestamp.h>
#include <mdebug.h>
#ifdef HAVE_STRING_H
#include <string.h>
#endif

extern int	optind;
extern char	*optarg;

static int	breakup_time(struct tm *tp, const char *ts) {
	char	*wc;
	int	l;

	/* timestamp must be complete					*/
	if ((l = strlen(ts)) < 12)
		return -1;

	/* create a local working copy					*/
	wc = (char *)alloca(l + 1);
	memcpy(wc, ts, l + 1);

	tp->tm_sec = 0; wc[12] = '\0';
	tp->tm_min = atoi(&wc[10]); wc[10] = '\0';
	tp->tm_hour = atoi(&wc[8]); wc[8] = '\0';
	tp->tm_mday = atoi(&wc[6]); wc[6] = '\0';
	tp->tm_mon = atoi(&wc[4]) - 1; wc[4] = '\0';
	tp->tm_year = atoi(wc) - 1900;
	tp->tm_yday = tp->tm_wday = tp->tm_isdst = 0;

	return 0;
}

time_t	localtime2time(const char *ts) {
	struct tm	tt;

	/* break up time						*/
	if (breakup_time(&tt, ts) < 0)
		return -1;

	/* make time from that						*/
	return mktime(&tt);
}

