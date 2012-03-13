/*
 * Rectangle.cc -- specify a rectangle from lower left point and upper
 *                 right point
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: Rectangle.cc,v 1.3 2009/01/10 19:00:25 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <Rectangle.h>
#include <mdebug.h>
#include <Configuration.h>

namespace meteo {

// construct a rectangle from the attributes llx, lly, urx and ury in an
// XML configuration file
Rectangle::Rectangle(const std::string& xpath) {
	Configuration	conf;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "constructing rectangle from %s",
		xpath.c_str());
	int	x, y;
	x = conf.getInt(xpath + "/@llx", 45);
	y = conf.getInt(xpath + "/@lly", 19);
	lowerleft = Point(x, y);
	x = conf.getInt(xpath + "/@urx", 445);
	y = conf.getInt(xpath + "/@ury", 139);
	upperright = Point(x, y);
}

} /* namespace meteo */
