/*
 * Timelabel.cc -- 
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung 
 */
#include <Timelabel.h>
#include <mdebug.h>
#include <MeteoException.h>
#include <iostream>

namespace meteo {

#define	YEAR_LENGTH	31556926
#define	MONTH_LENGTH	2629743
#define	WEEK_LENGTH	604800
#define	DAY_LENGTH	86400
#define	HOUR_LENGTH	3600

//////////////////////////////////////////////////////////////////////
// Level methods

Level::Level(const std::string& levelname) {
	if (levelname == "hour") {
		level = hour; return;
	}
	if (levelname == "day") {
		level = day; return;
	}
	if (levelname == "week") {
		level = week; return;
	}
	if (levelname == "month") {
		level = month; return;
	}
	if (levelname == "year") {
		level = year; return;
	}
}

Level::Level(const char c) {
	switch (c) {
	case 'h':	level = hour; break;
	case 'd':	level = day; break;
	case 'w':	level = week; break;
	case 'm':	level = month; break;
	case 'y':	level = year; break;
	default:
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "illegal label character: %c",
			c);
		throw MeteoException("illegal label character", "");
		break;
	}
}

Level::Level(const int interval) {
	switch (interval) {
	case 60:	level = hour; break;
	case 300:	level = day; break;
	case 1800:	level = week; break;
	case 7200: 	level = month; break;
	case 86400:	level = year; break;
	default:
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "illegal interval %d", interval);
		throw MeteoException("illegal interval: Level constructor", "");
		break;
	}
}

std::string	Level::getLevelString(void) const {
	switch (level) {
	case hour:	return "hour"; break;
	case day:	return "day"; break;
	case week:	return "week"; break;
	case month:	return "month"; break;
	case year:	return "year"; break;
	}
	// never reached, but keeps compiler happy
	return "";
}

int	Level::getInterval(void) const {
	switch (level) {
	case hour:	return 60; break;
	case day:	return 300; break;
	case week:	return 1800; break;
	case month:	return 7200; break;
	case year:	return 86400; break;
	}
	// never reached, but keeps compiler happy
	return -1;
}

int	Level::getDuration(void) const {
	switch (level) {
	case hour:	return HOUR_LENGTH; break;
	case day:	return DAY_LENGTH; break;
	case week:	return WEEK_LENGTH; break;
	case month:	return MONTH_LENGTH; break;
	case year:	return YEAR_LENGTH; break;
	}
	// never reached, but keesp compiler happy
	return day;
}

Level	Level::up(void) const {
	switch (level) {
	case hour:	return Level(day); break;
	case day:	return Level(week); break;
	case week:	return Level(month); break;
	case month:	return Level(year); break;
	default:
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "cannot go up");
		throw MeteoException("cannot go up", "");
		break;
	}
}

Level	Level::down(void) const {
	switch (level) {
	case year:	return Level(month); break;
	case month:	return Level(week); break;
	case week:	return Level(day); break;
	case day:	return Level(hour); break;
	default:
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "cannot go down");
		throw MeteoException("cannot go down", "");
		break;
	}
}

//////////////////////////////////////////////////////////////////////
// Timelabel methods

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

void	Timelabel::parse(const std::string& labelstring) {
	struct tm	tms;
	int		tm_week;
	tms.tm_isdst = 0;

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

	// extract remaining fields, depending on level
	switch (level.getLevel()) {
	case week:
		tm_week = atoi(labelstring.substr(5).c_str());
		tms.tm_mday = 7 * (tm_week - 1);
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

	// for the week, convert back to broken down time to 
	if (level.getLevel() == week) {
		struct tm	*tmp = localtime(&midtime);
		char	b[3];
		strftime(b, sizeof(b), "%V", tmp);
		midtime -= (7 * (atoi(b) - tm_week) + tmp->tm_wday - 3);
	}

	// normalize according to the rules of the normalize function
	midtime = normalize(midtime);
}

Timelabel::Timelabel(time_t t, const Level& l) : level(l) {
	midtime = normalize(t);
}

Timelabel::Timelabel(const std::string& datestring, const Level& l) : level(l) {
	// convert the timestring into struct tm and convert this to
	// time_t
	struct tm	tms;
	tms.tm_hour = 12; 
	tms.tm_min = 0; 
	tms.tm_sec = 0; 
	tms.tm_mday = atoi(datestring.substr(6, 2).c_str()); 
	tms.tm_mon = atoi(datestring.substr(4, 2).c_str()); 
	tms.tm_year = atoi(datestring.substr(0, 4).c_str()); 

	// normalize the converted time
	midtime = normalize(mktime(&tms));
}

Timelabel::Timelabel(const std::string& labelstring) {
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

	switch(level.getLevel()) {
	case hour:
		strftime(buffer, sizeof(buffer), "h%Y%m%d%H", tp);
		break;
	case day:
		strftime(buffer, sizeof(buffer), "d%Y%m%d", tp);
		break;
	case week:
		strftime(buffer, sizeof(buffer), "w%Y%V", tp);
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
