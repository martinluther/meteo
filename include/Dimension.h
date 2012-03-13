/*
 * Dimension.h -- a frame is something to draw into, it's the stuff around the
 *            the meteo data graph
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _Dimension_h 
#define _Dimension_h

#include <string>
#include <Point.h>

namespace meteo {

class Dimension {
	int	width, height;
public:
	Dimension(int w, int h) { width = w; height = h; }
	Dimension(const std::string& xpath);
	~Dimension(void) { }
	int	getWidth(void) const { return width; }
	int	getHeight(void) const { return height; }
	bool	containsPoint(const Point& p) {
		return ((p.getX() >= 0) && (p.getX() < width)
			&& (p.getY() >= 0) && (p.getY() < height));
	}
};

} /* namespace meteo */

#endif /* _Dimension_h */
