/*
 * Timelabel.cc -- 
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung 
 *
 * $Id: Timelabel.cc,v 1.8 2006/05/07 19:47:22 afm Exp $
 */
#include <Timelabel.h>
#include <mdebug.h>
#include <MeteoException.h>
#include <iostream>

namespace meteo {

// normalize -- normalize the time to one that can immediately be used to
//              compute the labelstring
//
// here is how normalization works: 
// 1. convert the time into broken down local time (since we want graphics
//    to be adjusted so that the middle of the day/week/month/year are in
//    the center of the graph with respect to local time)
// 2. compute the difference to the idealized middle 
// 3. compute new midtime
// 4. repeat until difference is 0
time_t	Timelabel::normalize(time_t t) const {
	// compute broken down time
	struct tm	*tmp = localtime(&t);

	// make a copy of broken down time, so that it does not matter if
	// we call mdebug (which itself calls localtime, overwriting tmp)
	struct tm	tms;
	memcpy(&tms, tmp, sizeof(tms));

	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "reducing   %d", t);

	// here are the time variables we are working with
	time_t		wth = t;
	time_t		wt;

	// how to compute difference depends on level:
	// hour:	reduce to the nearest hour
	// day:		reduce to the nearset midday
	// week:	reduce to the nearest wednesday
	// month:	reduce to the nearest 15th of the month
	// year:	reduce to the nearest 182th day of the year

	// first remove the influence of minutes and seconds
	wth -= (60 * tms.tm_min) + tms.tm_sec;

	// for day-year, also remove the influence of the hour, but always
	// go to middday
	wt = wth - 3600 * (tms.tm_hour - 12);

	// the rest depends on the level
	switch (level.getLevel()) {
	case hour:
		wt = wth;
		break;
	case day:
		break;
	case week:
		wt -= 86400 * (tms.tm_wday - 3);
		break;
	case month:
		wt -= 86400 * (tms.tm_mday - 15);
		break;
	case year:
		wt -= 86400 * (tms.tm_yday - 182);
		break;
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "reduced to %d", wt);

	// we are done if the thing does not change any more
	if (t == wt)
		return t;

	return normalize(wt);
}

// time for a given week number
static time_t	week2time(int year /* since 1900 */, int week) {
	struct tm	tms;
	tms.tm_isdst = 0;
	tms.tm_mon = 0;
	tms.tm_mday = 1;
	tms.tm_hour = 12; tms.tm_min = 0; tms.tm_sec = 0;
	tms.tm_year = year;

	// convert to time_t and back, mainly to get the week day of jan 1st
	time_t	t = mktime(&tms);
	struct tm	*tmp = localtime(&t);
	t += 86400 * (10 - tmp->tm_wday);
	tmp = localtime(&t); // first wednesday after or at jan 1st, or maybe
			// one week later, step (#) below will fix that
	
	char	b[3];
	strftime(b, sizeof(b), "%V", tmp);
	int	week0 = atoi(b);	// find week of t
	t += (week - week0) * 86400 * 7; // (#) fix week offset
	return t;
}

// parse a labelstring and convert it to a Timelabel object
void	Timelabel::parse(const std::string& labelstring) {
	struct tm	tms;
	struct tm	*tmp;
	time_t		t;
	int		tm_week;
	tms.tm_isdst = 0;

	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "parsing label: %s",
		labelstring.c_str());

	// the level can be derived from the first letter
	level = Level(labelstring[0]);

	// set default values
	tms.tm_mday = 14; // middle of a month
	tms.tm_mon = 0; // January
	tms.tm_hour = 12;
	tms.tm_min = 0;
	tms.tm_sec = 0;
	
	// extract year
	tms.tm_year = atoi(labelstring.substr(1, 4).c_str()) - 1900;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "parsed year: %d", tms.tm_year);

	// extract remaining fields, depending on level
	switch (level.getLevel()) {
	case week:
		tm_week = atoi(labelstring.substr(5).c_str());
		t = week2time(tms.tm_year, tm_week);
		tmp = localtime(&t);
		memcpy(&tms, tmp, sizeof(tms));
		break;
	case hour:
		tms.tm_hour = atoi(labelstring.substr(9, 2).c_str());
	case day:
		tms.tm_mday = atoi(labelstring.substr(7, 2).c_str());
	case month:
		tms.tm_mon = atoi(labelstring.substr(5, 2).c_str()) - 1;
		break;
	case year:
		break;
	}

	// turn this broken down time structure into a time value
	midtime = mktime(&tms);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "candidate midtime: %d", midtime);

	// normalize according to the rules of the normalize function
	midtime = normalize(midtime);
}

Timelabel::Timelabel(time_t t, const Level& l) : level(l) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "construct from time %d", t);
	midtime = normalize(t);
}

Timelabel::Timelabel(const std::string& datestring, const Level& l) : level(l) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "construct from datestring %s",
		datestring.c_str());
	// convert the timestring into struct tm and convert this to
	// time_t
	struct tm	tms;
	tms.tm_hour = 12; 
	tms.tm_min = 0; 
	tms.tm_sec = 0; 
	tms.tm_mday = atoi(datestring.substr(6, 2).c_str()); 
	tms.tm_mon = atoi(datestring.substr(4, 2).c_str()); 
	tms.tm_year = atoi(datestring.substr(0, 4).c_str()); 
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "%02d/%02d/%04d %02d:%02d:%02d",
		tms.tm_mday, tms.tm_mon, tms.tm_year,
		tms.tm_hour, tms.tm_min, tms.tm_sec);

	// normalize the converted time
	midtime = normalize(mktime(&tms));
}

Timelabel::Timelabel(const std::string& labelstring) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "construct from labelstring %s",
		labelstring.c_str());
	parse(labelstring);
}

Timelabel	Timelabel::previous(void) const {
	return Timelabel(midtime - level.getDuration(), level);
}

Timelabel	Timelabel::next(void) const {
	return Timelabel(midtime + level.getDuration(), level);
}

Timelabel	Timelabel::up(void) const {
	return Timelabel(midtime, level.up());
}

Timelabel	Timelabel::upup(void) const {
	return Timelabel(midtime, level.up().up());
}

Timelabel	Timelabel::upupup(void) const {
	return Timelabel(midtime, level.up().up().up());
}

Timelabel	Timelabel::down(void) const {
	return Timelabel(midtime, level.down());
}

Timelabel	Timelabel::downdown(void) const {
	return Timelabel(midtime, level.down().down());
}

Timelabel	Timelabel::downdowndown(void) const {
	return Timelabel(midtime, level.down().down().down());
}

std::string	Timelabel::getString(void) const {
	char	buffer[64];
	struct tm	*tp = localtime(&midtime);
	int	w, y;

	switch(level.getLevel()) {
	case hour:
		strftime(buffer, sizeof(buffer), "h%Y%m%d%H", tp);
		break;
	case day:
		strftime(buffer, sizeof(buffer), "d%Y%m%d", tp);
		break;
	case week:
		y = tp->tm_year;
		strftime(buffer, sizeof(buffer), "%V", tp);
		w = atoi(buffer);
		if ((tp->tm_mon < 6) && (w > 26)) {
			y = tp->tm_year - 1;
		}
		if ((tp->tm_mon > 6) && (w < 26)) {
			y = tp->tm_year + 1;
		}
		snprintf(buffer, sizeof(buffer), "w%04d%02d", y + 1900, w);
		break;
	case month:
		strftime(buffer, sizeof(buffer), "m%Y%m", tp);
		break;
	case year:
		strftime(buffer, sizeof(buffer), "y%Y", tp);
		break;
	}
	return std::string(buffer);
}

std::string	Timelabel::getTitle(void) const {
	char	buffer[64];
	struct tm	*tp = localtime(&midtime);

	switch(level.getLevel()) {
	case hour:
		strftime(buffer, sizeof(buffer), "%c", tp);
		break;
	case day:
		strftime(buffer, sizeof(buffer), "%A, %x", tp);
		break;
	case week:
		strftime(buffer, sizeof(buffer), "week %V %Y", tp);
		break;
	case month:
		strftime(buffer, sizeof(buffer), "%B %Y", tp);
		break;
	case year:
		strftime(buffer, sizeof(buffer), "%Y", tp);
		break;
	}
	return std::string(buffer);
}

std::string	Timelabel::getCtime(void) const {
	return ctime(&midtime);
}

} /* namespace meteo */

