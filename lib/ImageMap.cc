/*
 * ImageMap.cc -- implementation of image map methods
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: ImageMap.cc,v 1.2 2004/02/25 23:48:05 afm Exp $
 */
#include <ImageMap.h>
#include <mdebug.h>
#include <MeteoException.h>
#include <algo.h>

namespace meteo {

// the DisplayMap class ise used internally in the ImageMap class, since the
// for_each algorithm needs a functor object
class	DisplayMap {
	std::string	mapstring;
	std::string	baseurl;
public:
	DisplayMap(const std::string& url) { baseurl = url; }
	~DisplayMap(void) { }
	void	operator()(const MapArea& m) {
		mapstring += m.getStringForm(baseurl);
	}
	std::string	getStringForm(void) const { return mapstring; }
};

std::string	ImageMap::getStringForm(const std::string& url) const {
	std::string	result;
	result = "<map name=\"" + level.getLevelString() + "map\">\n";
	DisplayMap d = for_each(areas.begin(), areas.end(), DisplayMap(url));
	result += d.getStringForm();
	result += "</map>\n";
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "map string: %s", result.c_str());
	return result;
}

std::string	ImageMap::getImageTag(const std::string& filename) const {
	std::string	result = "<img src=\"";
	result += filename + "\" ";
	result += "usemap=\"#" + level.getLevelString() + "map\" ";
	result += "border=\"0\" ";
	return result + " />\n";
}

} /* namespace meteo */
