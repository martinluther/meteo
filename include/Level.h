/*
 * Level.h -- a helper class to the timelabel, represents the level in the
 *            time hierarchy
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung 
 */
#ifndef _Level_h
#define _Level_h

#include <string>

namespace meteo {

typedef enum level_e {
	hour = 0, day = 1, week = 2, month = 3, year = 4
}	level_t;

class	Level {
	level_t	level;
public:
	Level(void) { level = day; }
	Level(level_t l) : level(l) { }
	Level(int interval);
	Level(const std::string& levelname);
	Level(const char c);
	~Level(void) { }
	level_t	getLevel(void) const { return level; }
	std::string	getLevelString(void) const;
	int	getInterval(void) const;
	int	getDuration(void) const;
	Level	up(void) const;
	Level	down(void) const;
};

} /* namespace meteo */

#endif /* _Level_h */
