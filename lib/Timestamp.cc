/*
 * Timestamp.cc -- timestamp class for conversion of localtime to time
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung 
 *
 * $Id: Timestamp.cc,v 1.6 2009/01/10 19:00:25 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <Timestamp.h>
#include <MeteoException.h>
#include <time.h>
#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif /* HAVE_STDIO_H */
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif /* HAVE_STDLIB_H */
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */
#ifdef HAVE_STRING_H
#include <string.h>
#endif /* HAVE_STRING_H */
#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#endif /* HAVE_ALLOCA_H */

namespace meteo {

/*
 * breakup_time -- break a timestamp in the form YYYYMMDDhhmmsss to struct tm
 *
 * this function only takes care of the numeric conversions, it does not do
 * any verification. The localtime2time function below is supposed to
 * compute actual time_t values.
 */
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

/*
 * localtime2time -- convert a timestamp string of the form YYYYMMDDhhmmss
 *                   to local time
 *
 * the main problem in this method is that the timestamp strings include 
 * daylight savings time, but the time_t returned should be corrected for
 * that. A correct version was found by Eric Varsanyi. Thanks Eric!
 * The main idea is to do the mktime conversion twice: the first call sets
 * the tm_isdst flag to the correct value, and with the second call the
 * conversion takes place using this correct tm_isdst.
 */
static time_t	localtime2time(const char *ts) {
	struct tm	tt;

	/* break up time						*/
	memset(&tt, 0, sizeof(tt));	// bug pointed out by eric varsanyi
	if (breakup_time(&tt, ts) < 0)
		return -1;

	/* Get daylight savings, mktime() will 'adjust' the hour if it
	   found that the target date was DST (since we started off saying
	   is_dst == 0). This will give as the correct DST setting	*/
	mktime(&tt);

	/* new we have to break up the time again and we'll get the actual
	   time requested on that date (rather than adjusted an hour if the
	   date was DST).						*/
	if (breakup_time(&tt, ts) < 0)
		return -1;
	return mktime(&tt);
}

Timestamp::Timestamp(const std::string& ts) {
	t = localtime2time(ts.c_str());
}

Timestamp::Timestamp() {
	time(&t);
}

std::string	Timestamp::ctime() const {
	return std::string(::ctime(&t));
}

std::string	Timestamp::strftime(const char *format) const {
	struct tm	*tp = localtime(&t);
	char	buffer[1024];
	if (::strftime(buffer, sizeof(buffer), format, tp) <= 0) {
		throw MeteoException("cannot format time", std::string(format));
	}
	return std::string(buffer);
}

} /* namespace meteo */
