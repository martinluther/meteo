/*
 * GraphWindow.h -- abstracts graphs inside windows
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _GraphWindow_h
#define _GraphWindow_h

#include <Frame.h>
#include <Tdata.h>
#include <math.h>
#include <Configuration.h>
#include <Dataset.h>
#include <Timelabel.h>
#include <list>

namespace meteo {

// Scale objects are used to convert application domain data into data useful
// for graphing. After the object has been initialized with the min and max
// values, it will always return a value between 0 and 1, so that 0 corresponds
// to the minimum, 1 corresponds to the maximum
class Scale {
	double  a, b;
public:
	Scale(void) { a = 1.; b = 0.; } // if nothing is specified, use id map
	Scale(double min, double max) {
		a = 1/(max - min);
		b = min;
	}
	Scale(const Configuration& conf, const std::string& xpath,
		const Dataset& ds);
	~Scale(void) { }
	double  operator()(double x) const {
		double	y = a * (x - b);
		if (y > 1.) return 1.;
		if (y < 0.) return 0.;
		return y;
	}
	double	getMin(void) const { return b; }
	double	getMax(void) const { return b + 1/a; }
	double	range(void) const { return 1/a; }
};

class	GraphPoint {
	time_t	t;
	double	v;
public:
	GraphPoint(void) { t = 0; v = 0.; }
	GraphPoint(time_t tt, double vv) { t = tt; v = vv; }
	~GraphPoint(void) { }
	time_t	getTime(void) const { return t; }
	double	getValue(void) const { return v; }
};

class Axis {
	std::string	format;
	double		first, last, step;
	bool		gridlines, ticks;
public:
	Axis(void) : format("%.0f") {	
		first = 0.; last = 1.; step = 1.;
		gridlines = false; ticks = false;
	}
	Axis(std::string& f, double fi, double la, double st) : format(f) {
		first = fi; last = la; step = st;
	}
	Axis(const Configuration& cont, const std::string& xpath,
		const Scale& sc);
	~Axis(void) { }
	void	setFirst(double f) { first = f; }
	void	setLast(double l) { last = l; }
	void	setStep(double s) { step = s; }
	void	setFormat(const std::string& f) { format = f; }
	void	setGridlines(bool g) { gridlines = g; }
	void	setTicks(bool t) { ticks = t; }

	double	getFirst(void) const { return first; }
	double	getLast(void) const { return last; }
	double	getStep(void) const { return step; }
	const std::string& 	getFormat(void) const { return format; }
	bool	getGridlines(void) const { return gridlines; }
	bool	getTicks(void) const { return ticks; }
};

// MapArea -- describes the area of a month, week or day on year, month
//            or week graphs. The meteobrowser uses this information when
//            building the web page
class	MapArea {
	Rectangle	area;
	Timelabel	ti;
	time_t		starttime;
public:
	MapArea(const Rectangle& outline, const Timelabel& tl, time_t start)
		: area(outline) {
		ti = tl;
		starttime = start;
	}
	~MapArea(void) { }
	time_t		getStarttime(void) const { return starttime; }
	Timelabel	getTimelabel(void) const { return ti; }
	std::string	getStringForm(const std::string& url) const;
};

// ImageMap -- describes an image map, including a list of MapArea objects
class	ImageMap {
	std::list<MapArea>	areas;
	Level	level;
public:
	ImageMap(void) {
		level = Level(month);
	}
	ImageMap(const Level& l) {
		level = l;
	}
	~ImageMap(void) { }
	Level	getLevel(void) const { return level; }
	void	setLevel(const Level& l) { level = l; }
	void	setLevel(const int interval) { level = Level(interval); }
	void	addArea(const MapArea& a) {
		if (a.getStarttime() < time(NULL)) areas.push_back(a);
	}
	std::string	getStringForm(const std::string& url) const;
	std::string	getImageTag(const std::string& filename) const;
};

class GraphWindow {
	Frame&		parent;
	Rectangle	window;
	int		interval;
	time_t		starttime, endtime;
	Scale		leftscale, rightscale;
	Axis		leftaxis, rightaxis;
	ImageMap	imap;
public:
	GraphWindow(Frame& p, const Rectangle& r) : parent(p), window(r)  {
		time_t	e;
		time(&e);
		setEndTime(e, 300);
	}
	~GraphWindow(void) { }

	// methods to specify the time scale 
	void	setEndTime(time_t e, int i) {
		interval = i;
		imap.setLevel(i);
		endtime = e - (e % interval);
		starttime = endtime - getWidth() * interval;
	}
	void	setStartTime(time_t s, int i) {
		interval = i;
		imap.setLevel(i);
		starttime = s - (s % interval);
		endtime = s + getWidth() * interval;
	}
	int	getInterval(void) const { return interval; }
	time_t	getEndTime(void) const { return endtime; }
	time_t	getStartTime(void) { return starttime; }
	int	getWidth(void) const { return window.getWidth(); }
	int	getIndexFromTime(time_t t) const {
		return (t - starttime) / interval;
	}
	int	getXFromTime(time_t t) const {
		return window.getLeft() + getIndexFromTime(t);
	}
	time_t	getTimeFromIndex(int i) const {
		return starttime + i * interval;
	}

	// handle vertical scales
	int	getHeight(void) const { return window.getHeight(); }
	void	setLeftScale(const Scale& l) { leftscale = l; }
	void	setRightScale(const Scale& r) { rightscale = r; }
	Scale	getLeftScale(void) { return leftscale; }
	Scale	getRightScale(void) { return rightscale; }
	int	getY(bool useleftscale, double value) const {
		double	y = (useleftscale) ? leftscale(value)
					   : rightscale(value);
		return (int)(y * window.getHeight() + window.getLow());
	}
	int	getX(time_t t) const {
		return window.getLeft() + getIndexFromTime(t);
	}

	// accessors for axis
	void	setLeftAxis(const Axis& l) { leftaxis = l; }
	const Axis&	getLeftAxis(void) const { return leftaxis; }
	void	setRightAxis(const Axis& r) { rightaxis = r; }
	const Axis&	getRightAxis(void) const { return rightaxis; }

	// point conversions from Window coordinates (time_t, double)
	// to frame coordinates (int, int)
	Point	getPoint(bool useleftscale, const GraphPoint& gp) const {
		return Point(getX(gp.getTime()),
			getY(useleftscale, gp.getValue()));
	}
	Point	getBottomPoint(time_t t) const {
		return Point(window.getRight() - (endtime - t)/interval,
			window.getLow());
	}
	Point	getTopPoint(time_t t) const {
		return Point(window.getRight() - (endtime - t)/interval,
			window.getHigh());
	}

	// get a color from a string, or use the foreground for the present
	// image
	Color	getColorFromHexString(const std::string& hexcolorspec) const {
		return parent.getColorFromHexString(hexcolorspec);
	}

	// handle graphing, single points
	void	drawHistogram(bool useleftscale, const GraphPoint& gp,
		const Color& color) {
		parent.drawLine(getPoint(useleftscale, gp),
			getBottomPoint(gp.getTime()), color, solid);
	}
	void	drawLine(bool useleftscale, const GraphPoint& gp1,
			const GraphPoint& gp2, const Color& color,
			linestyle style) {
		parent.drawLine(getPoint(useleftscale, gp1),
			getPoint(useleftscale, gp2), color, style);
	}
	void	drawRectangle(bool useleftscale, const GraphPoint& lowerleft,
		const GraphPoint& upperright, const Color& color) {
		parent.drawRectangle(Rectangle(
			getPoint(useleftscale, lowerleft),
			getPoint(useleftscale, upperright)), color);
	}
	void	drawText(bool useleftscale, const std::string& text,
			const GraphPoint& start, const Color& color,
			bool horizontal = true) {
		parent.drawText(text, getPoint(useleftscale, start), color,
			horizontal);
	}
	void	drawLetter(bool useleftscale, char c, const GraphPoint& center, 
			const Color& color) {
		parent.drawLetter(c, getPoint(useleftscale, center), color);
	}

	// handle graphing for complete time series, i.e. maps time_t -> double
	void	drawLine(bool useleftscale, const Tdata&, const Color&,
			linestyle style);
	void	drawHistogram(bool useleftscale, const Tdata&, const Color&);
	void	drawRange(bool useleftscale, const Tdata& lower,
			const Tdata& upper, const Color&);
	void	drawNodata(const Tdata&, const Color&);
	void	drawWindBackground(bool useleftscale, 
			const Color& northcolor, const Color& southcolor,
			const Color& westcolor, const Color& eastcolor);
	void	drawWindBackgroundLetters(bool useleftscale,
			const std::string& letters);
	
	// handle drawing grids
private:
	void	drawVerticalLine(time_t t, linestyle style);
	void	drawTimeTick(time_t t);
	void	drawTimeLabel(time_t t, const std::string& label);
	void	drawValueGridlines(bool useleftscale);
	void	drawValueTicks(bool useleftscale);
public:
	MapArea	getArea(time_t start, time_t end) const;
	void	drawTimeGrid(void);
	void	drawValueGrid(void);

	// draw a frame around the graph window
	void	drawFrame(void);
	std::string	mapString(const std::string& url) const;
	std::string	imageTagString(const std::string& filename) const;
};

} /* namespace meteo */

#endif /* _GraphWindow_h */
