/*
 * Dimension.cc -- the Dimension class represents sizes of rectangular areas
 *                 without actually drawing rectangles
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#include <Dimension.h>
#include <mdebug.h>
#include <Configuration.h>

namespace meteo {

// create a dimension from the height and width attributes in an XML
// configuration file
Dimension::Dimension(const std::string& xpath) {
	Configuration	conf;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "constructing Dimension from %s",
		xpath.c_str());
	height = conf.getInt(xpath + "/@height", 144);
	width = conf.getInt(xpath + "/@width", 500);
}

} /* namespace meteo */
