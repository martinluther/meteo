/*
 * MapArea.h -- abstracts map areas for the use in image maps
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: MapArea.h,v 1.2 2004/02/25 23:52:34 afm Exp $
 */
#ifndef _MapArea_h
#define _MapArea_h

#include <Rectangle.h>
#include <Timelabel.h>
#include <string>
#include <time.h>

namespace meteo {
// MapArea -- describes the area of a month, week or day on year, month
//            or week graphs. The meteobrowser uses this information when
//            building the web page
class	MapArea {
	Rectangle	area;
	Timelabel	ti;
	time_t		starttime;
public:
	MapArea(const Rectangle& outline, const Timelabel& tl, time_t start)
		: area(outline) {
		ti = tl;
		starttime = start;
	}
	~MapArea(void) { }
	time_t		getStarttime(void) const { return starttime; }
	Timelabel	getTimelabel(void) const { return ti; }
	std::string	getStringForm(const std::string& url) const;
};

} /* namespace meteo */

#endif /* _MapArea_h */
