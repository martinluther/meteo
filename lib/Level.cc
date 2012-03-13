/*
 * Level.cc -- represent levelinside time hierarchy
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung 
 *
 * $Id: Level.cc,v 1.2 2004/02/25 23:48:05 afm Exp $
 */
#include <Level.h>
#include <mdebug.h>
#include <MeteoException.h>
#include <iostream>

namespace meteo {

#define	YEAR_LENGTH	31556926
#define	MONTH_LENGTH	2629743
#define	WEEK_LENGTH	604800
#define	DAY_LENGTH	86400
#define	HOUR_LENGTH	3600

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

} /* namespace meteo */

