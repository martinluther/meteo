/*
 * Frame.h -- a frame is something to draw into, it's the stuff around the
 *            the meteo data graph
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _Frame_h 
#define _Frame_h

#include <Configuration.h>
#include <string>
#include <time.h>

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

class Dimension {
	int	width, height;
public:
	Dimension(int w, int h) { width = w; height = h; }
	Dimension(const Configuration& conf, const std::string& xpath);
	~Dimension(void) { }
	int	getWidth(void) const { return width; }
	int	getHeight(void) const { return height; }
	bool	containsPoint(const Point& p) {
		return ((p.getX() >= 0) && (p.getX() < width)
			&& (p.getY() >= 0) && (p.getY() < height));
	}
};

class Rectangle {
	Point	lowerleft;
	Point	upperright;
public:
	Rectangle(void) : lowerleft(0, 0), upperright(0, 0) { }
	Rectangle(const Point& ll, const Point& ur)
		: lowerleft(ll), upperright(ur) {
	}
	Rectangle(const Configuration& conf, const std::string& xpath);
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

class Color {
	int	red, green, blue;
public:
	Color(void) { red = 255; green = 255; blue = 255; } // default white
	Color(const std::string& hexcolorspec);
	Color(int r, int g, int b) { red = r; green = g; blue = b; }
	int	getRed(void) const { return red; }
	int	getGreen(void) const { return green; }
	int	getBlue(void) const { return blue; }
	int	getValue(void) const {
		return (red << 16) + (green << 8) + blue;
	}
	std::string	getHex(void) const;
	~Color(void) { }
	bool	operator<(const Color& c) const {
		return (getValue() < c.getValue());
	}
};

typedef enum labelalign_e {
	top, center, bottom
} 	labelalign_t;

class Label {
	std::string	text;
	labelalign_t	align;
public:
	Label(const std::string& t, labelalign_t a) : text(t), align(a) { }
	Label(const Configuration& conf, const std::string& xpath);
	~Label(void) { }
	const std::string&	getText(void) const { return text; }
	const labelalign_t	getAlign(void) const { return align; }
};

class	frame_internals;

typedef enum linestyle_e {
	solid = 0, dotted = 1, dashed = 2
} linestyle;

class Frame {
	Dimension	outer;
	frame_internals	*fi;
	Color		foreground, background;
	// construct the internal GD stuff
	void	setupInternals(void);
public:
	// construction an destruction
	Frame(const Dimension& o);
	Frame(const Frame& f);
	~Frame(void);
	Frame&	operator=(const Frame& other);

	// foreground and background colors
	const Color&	getForeground(void) const { return foreground; }
	const Color&	getBackground(void) const { return background; }
	void	setForeground(const Color& c);
	void	setForeground(const std::string& c);
	void	setBackground(const Color& c);
	void	setBackground(const std::string& c);
	Color	getColorFromHexString(const std::string& c) const;

	// drawing functions (this functions perform some limited form
	// of clipping)
	void	drawRectangle(const Rectangle& rectangle, const Color& color);
	void	drawLine(const Point& p1, const Point& p2, const Color& color,
			linestyle style);
	void	drawText(const std::string& text, const Point& start,
			const Color& color, bool horizontal = true);
	void	drawLetter(const char c, const Point& center,
			const Color& color);

	// specialised drawing functions
	void	drawLabel(const std::string& text, bool left, labelalign_t align);
	void	drawLabel(const Label& label, bool left);

	// output of image to a file
	void	toFile(const std::string& filename);
};

} /* namespace meteo */

#endif /* _Frame_h */
