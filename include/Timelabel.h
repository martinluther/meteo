/*
 * Timelabel.h -- a class to make the idea of the `middle' of a day, week,
 *                month or year canonical
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung 
 *
 * $Id: Timelabel.h,v 1.6 2004/02/25 23:52:35 afm Exp $
 */
#ifndef _Timelabel_h
#define _Timelabel_h

#include <Level.h>
#include <string>
#include <time.h>

namespace meteo {

class	Timelabel {
	time_t		midtime;
	Level		level;

	time_t	normalize(time_t t) const;
	void	parse(const std::string& labelstring);
public:
	// constructors
	Timelabel(void) : level(day) { midtime = normalize(time(NULL)); }
	Timelabel(time_t t, const Level& l);
	Timelabel(const std::string& datestring, const Level& l);
	Timelabel(const std::string& labelstring);
	~Timelabel(void) { }

	// accessors
	std::string	getString(void) const;
	time_t	getEndtime(int width) const;
	time_t	getTime(void) const { return midtime; }
	int	getInterval(void) const { return level.getInterval(); }
	Level	getLevel(void) const { return level; }
	std::string	getCtime(void) const;
	std::string	getTitle(void) const;

	// navigation
	Timelabel	next(void) const;
	Timelabel	previous(void) const;
	Timelabel	up(void) const;
	Timelabel	upup(void) const;
	Timelabel	upupup(void) const;
	Timelabel	down(void) const;
	Timelabel	downdown(void) const;
	Timelabel	downdowndown(void) const;
};

} /* namespace meteo */

#endif /* _Timelabel_h */
