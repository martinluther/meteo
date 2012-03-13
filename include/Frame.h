/*
 * Frame.h -- a frame is something to draw into, it's the stuff around the
 *            the meteo data graph
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _Frame_h 
#define _Frame_h

#include <string>
#include <map.h>
#include <time.h>

namespace meteo {

class Dimension {
	int	width, height;
public:
	Dimension(int w, int h) { width = w; height = h; }
	~Dimension(void) { }
	int	getWidth(void) const { return width; }
	int	getHeight(void) const { return height; }
};

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

class Scale {
	double	a, b;
public:
	Scale(void) { a = 1.; b = 0.; }	// if nothing is specified, use id map
	Scale(double aa, double bb) { a = aa; b = bb; }
	~Scale(void) { }
	double	operator()(double x) const { return a * x + b; }
};

class Color {
	int	red, green, blue;
public:
	Color(const std::string& hexcolorspec);
	Color(int r, int g, int b) { red = r; g = green; b = blue; }
	int	getRed(void) const { return red; }
	int	getGreen(void) const { return green; }
	int	getBlue(void) const { return blue; }
	~Color(void) { }
};

class Spacing {
	std::string&	format;
	double		first, last, step;
public:
	Spacing(std::string& f, double fi, double la, double st) : format(f) {
		first = fi; last = la; step = st;
	}
	~Spacing(void) { }
};

typedef	map<time_t, double>	tdata;	// undefined values are represented
					// by missing time_t

class	frame_internals;

class Frame {
	Dimension	outer, inner;
	Point		lowerleft;
	int		interval;
	time_t		rightend;
	Scale		leftscale, rightscale;
	frame_internals	*fi;
public:
	// construction an destruction
	Frame(const Dimension& o, const Dimension& i, const Point& l, int iv)
		: outer(o), inner(i), lowerleft(l) { interval = iv; }
	~Frame(void) { }

	// create labels left and right
	void	leftLabel(const std::string& label);
	void	rightLabel(const std::string& label);

	// create scales
	Scale	getScale(double minvalue, double maxvalue) const;
	Scale	getScaleStep(double minvalue, double step) const;
	void	setLeftScale(const Scale& s) { leftscale = s; }
	void	setRightScale(const Scale& s) { rightscale = s; }

	// time axis computations
	int	getIndexFromTime(time_t) const;
	time_t	getTimeFromIndex(int i) const;
	void	setRightEnd(time_t r) { rightend = r; }
	void	setLeftEnd(time_t l) {
		rightend = l + interval * inner.getWidth();
	}
	time_t	getLeftEnd(void) const;
	time_t	getRightEnd(void) const;
	int	getInterval(void) const { return interval; }
	void	setInterval(int i) { interval = i; }

	// create points based on time and values
	Point	getPoint(bool useleftscale, time_t t, double value) const;
	Point	getBottomPoint(time_t t) const;
	Point	getTopPoint(time_t t) const;

	// drawing functions (this functions perform some limited form
	// of clipping)
	void	drawRectangle(const Point& lowerleft, const Point& upperright,
		const Color& color);
	void	drawRange(const Point& lower, const Point& upper,
		const Color& color);
	void	drawHistogram(const Point&, const Color& color);
	void	drawLine(const Point& p1, const Point& p2, const Color& color);
	void	drawVLine(bool useleftscale, const tdata& data,
		const Color& color);
	void	drawHistogram(bool useleftscale, const map<time_t, double>& data,
		const Color& color);
	void	drawRange(bool useleftscale, const tdata& upperdata,
		const tdata& lowerdata, const Color& color);
	void	drawNodata(const tdata& data, const Color& color);

	// grid related methods
	void	drawHGrid(bool useleftscale, const Spacing& spc);
	void	drawVGrid(void);

	// output of image to a file
	void	toFile(const std::string& filename);
};

} /* namespace meteo */

#endif /* _Frame_h */
