/*
 * ImageMap.h -- abstracts graphs inside windows
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: ImageMap.h,v 1.2 2004/02/25 23:52:34 afm Exp $
 */
#ifndef _ImageMap_h
#define _ImageMap_h

#include <MapArea.h>
#include <Level.h>
#include <list>

namespace meteo {

// ImageMap -- describes an image map, including a list of MapArea objects
class	ImageMap {
	std::list<MapArea>	areas;
	Level	level;
public:
	ImageMap(void) {
		level = Level(month);
	}
	ImageMap(const Level& l) {
		level = l;
	}
	~ImageMap(void) { }
	Level	getLevel(void) const { return level; }
	void	setLevel(const Level& l) { level = l; }
	void	setLevel(const int interval) { level = Level(interval); }
	void	addArea(const MapArea& a) {
		if (a.getStarttime() < time(NULL)) areas.push_back(a);
	}
	std::string	getStringForm(const std::string& url) const;
	std::string	getImageTag(const std::string& filename) const;
};

} /* namespace meteo */

#endif /* _ImageMap_h */
