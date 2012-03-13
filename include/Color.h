/*
 * Color.h -- a frame is something to draw into, it's the stuff around the
 *            the meteo data graph
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _Color_h 
#define _Color_h

#include <string>
#include <time.h>

namespace meteo {

class Color {
	int	red, green, blue, alpha;
public:
	Color(void) { red = 255; green = 255; blue = 255; alpha = 0; } // default white
	Color(const std::string& hexcolorspec);
	Color(int r, int g, int b) { red = r; green = g; blue = b; }
	Color(int r, int g, int b, int a) {
		red = r; green = g; blue = b; alpha = a;
	}
	int	getRed(void) const { return red; }
	int	getGreen(void) const { return green; }
	int	getBlue(void) const { return blue; }
	int	getAlpha(void) const { return alpha; }
	int	getValue(void) const;
	std::string	getHex(void) const;
	~Color(void) { }
	bool	operator<(const Color& c) const {
		return (getValue() < c.getValue());
	}
};

} /* namespace meteo */

#endif /* _Color_h */
