/*
 * Rectangle.h -- a rectangle is defined by the two points lowerleft and
 *                upperright
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: Rectangle.h,v 1.2 2004/02/25 23:52:35 afm Exp $
 */
#ifndef _Rectangle_h 
#define _Rectangle_h

#include <Point.h>
#include <string>

namespace meteo {

class Rectangle {
	Point	lowerleft;
	Point	upperright;
public:
	Rectangle(void) : lowerleft(0, 0), upperright(0, 0) { }
	Rectangle(const Point& ll, const Point& ur)
		: lowerleft(ll), upperright(ur) {
	}
	Rectangle(const std::string& xpath);
	~Rectangle(void) { }
	int	getWidth(void) const {
		return upperright.getX() - lowerleft.getX();
	}
	int	getHeight(void) const {
		return upperright.getY() - lowerleft.getY();
	}
	const Point&	getLowerLeft(void) const { return lowerleft; }
	const Point&	getUpperRight(void) const { return upperright; }
	int	getLow(void) const { return lowerleft.getY(); }
	int	getHigh(void) const { return upperright.getY(); }
	int	getLeft(void) const { return lowerleft.getX(); }
	int	getRight(void) const { return upperright.getX(); }
	bool	containsPoint(const Point& p) {
		return ((lowerleft.getX() <= p.getX()) &&
			(p.getX() < upperright.getX()) &&
			(lowerleft.getY() <= p.getY()) &&
			(p.getY() < upperright.getY()));
	}
};

} /* namespace meteo */

#endif /* _Rectangle_h */
