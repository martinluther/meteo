/*
 * Point.h -- basic class that represents points in the coordinate system of
 *            a frame
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _Point_h 
#define _Point_h

namespace meteo {

// Points use the coordinate system of the inner frame (so the dont remember
// the lower left point where the inner frame is attached).
class Point {
	int	x, y;
public:
	Point(void) { x = 0; y = 0; }
	Point(int xx, int yy) { x = xx; y = yy; }
	int	getX(void) const { return x; }
	int	getY(void) const { return y; }
	~Point(void) { }
};

} /* namespace meteo */

#endif /* _Point_h */
