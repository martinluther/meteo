/*
 * Color.cc -- the color object represents the RGB color of an object to
 *             be drawn with gd.
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: Color.cc,v 1.3 2009/01/10 19:00:23 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <Color.h>
#include <mdebug.h>
#include <MeteoException.h>
#include <gd.h>
#include <mdebug.h>
#ifdef HAVE_STRING_H
#include <string.h>
#endif /* HAVE_STRING_H */
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif /* HAVE_STDLIB_H */

namespace meteo {

// create a Color from a hex string. Note that this extends X11 or web color
// strings: there may be a fourth byte representing alpha
Color::Color(const std::string& hexcolorspec) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "construct color %s",
		hexcolorspec.c_str());
	std::string	cs = hexcolorspec;
	// remove leading hash character if present
	if (hexcolorspec[0] == '#') {
		cs = hexcolorspec.substr(1);
	}
	// check that the length is ok, throw exception otherwise
	if ((cs.length() != 6) && (cs.length() != 8)) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "illegal web color "
			"specification: [%s]", hexcolorspec.c_str());
		throw MeteoException("illegal color specification",
			"[" + hexcolorspec + "]");
	}

	// split the color specification into three strings, and convert
	// every piece from hex to decimal
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "converting %s to rgb", cs.c_str());
	red = strtol(cs.substr(0, 2).c_str(), NULL, 16);
	green = strtol(cs.substr(2, 2).c_str(), NULL, 16);
	blue = strtol(cs.substr(4, 2).c_str(), NULL, 16);

	// retrieve the alpha channel if present
	if (cs.length() == 8) {
		alpha = strtol(cs.substr(6, 2).c_str(), NULL, 16);
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "%s -> %d/%d/%d/%d",
			hexcolorspec.c_str(), red, green, blue, alpha);
	} else {
		alpha = 0;
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "%s -> %d/%d/%d",
			hexcolorspec.c_str(), red, green, blue);
	}
}

// retrieve the GD color index associate with the RGBA values in the Color
// object
int	Color::getValue(void) const {
	return gdTrueColorAlpha(red, green, blue, alpha);
}

// retrieve the Hex representation of the color, including alpha
std::string	Color::getHex(void) const {
	char	buffer[80];
	snprintf(buffer, sizeof(buffer), "#%02x%02x%02x", red, green, blue);
	if (alpha > 0) {
		snprintf(buffer + 7, sizeof(buffer) - 7, "%02x", alpha);
	}
	return std::string(buffer);
}

} /* namespace meteo */
