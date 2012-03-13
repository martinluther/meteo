/*
 * MapArea.cc -- implementation of MapArea methods
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: MapArea.cc,v 1.2 2004/02/25 23:48:05 afm Exp $
 */
#include <MapArea.h>
#include <mdebug.h>

namespace meteo {

// turn a map area into a string representation of the map
// @baseurl  	must contain language, station name and level
std::string	MapArea::getStringForm(const std::string& baseurl) const {
	std::string	result;
	char	buffer[1024];
	snprintf(buffer, sizeof(buffer), "<area coords=\"%d,%d,%d,%d\" ",
		area.getLeft(), area.getLow(), area.getRight(), area.getHigh());
	result = std::string(buffer);
	snprintf(buffer, sizeof(buffer), "alt=\"%s\" ",
		ti.getString().c_str());
	result += buffer;
	snprintf(buffer, sizeof(buffer), "href=\"%s&label=%s\" />\n",
		baseurl.c_str(), ti.getString().c_str());
	result += buffer;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "area string is %s", result.c_str());
	return result;
}

} /* namespace meteo */
