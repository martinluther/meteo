/*
 * Dimension.cc -- the Dimension class represents sizes of rectangular areas
 *                 without actually drawing rectangles
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: Dimension.cc,v 1.3 2009/01/10 19:00:24 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
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
