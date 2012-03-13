/*
 * GraphWindow.h -- abstracts graphs inside windows
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: GraphWindow.h,v 1.13 2004/02/27 22:09:04 afm Exp $
 */
#ifndef _GraphWindow_h
#define _GraphWindow_h

#include <Frame.h>
#include <Scale.h>
#include <Axis.h>
#include <GraphPoint.h>
#include <ImageMap.h>

namespace meteo {

class GraphWindow {
	Frame&		parent;
	Rectangle	window;
	int		offset;
	int		interval;
	// starttime and endtime are times such that starttime + offset
	// is divisible by the interval. this ensures that all numbers
	// of the form starttime + i * interval + offset are also valid
	// timekeys.
	time_t		starttime, endtime;
	Scale		leftscale, rightscale;
	Axis		leftaxis, rightaxis;
	ImageMap	imap;
public:
	GraphWindow(Frame& p, const Rectangle& r, int off, int i) : parent(p),
		window(r), offset(off), interval(i)  {
		imap.setLevel(interval);
		time_t	e;
		time(&e);
		setEndTime(e);
	}
	~GraphWindow(void) { }

	// method to reduce the time to the grid.
	time_t	reduceTime(time_t t) const;

	// methods to specify the time scale 
	void	setEndTime(time_t e) {
		endtime = reduceTime(e);
		starttime = endtime - getWidth() * interval;
	}
	void	setStartTime(time_t s) {
		starttime = reduceTime(s);
		endtime = starttime + getWidth() * interval;
	}
	int	getInterval(void) const { return interval; }
	int	getOffset(void) const { return offset; }
	time_t	getEndTime(void) const { return endtime; }
	time_t	getEndTimekey(void) const { return getEndTime() + offset; }
	time_t	getStartTime(void) { return starttime; }
	time_t	getStartTimekey(void) { return getStartTime() + offset; }
	int	getWidth(void) const { return window.getWidth(); }
	int	getIndexFromTime(time_t t) const {
		return (reduceTime(t) - starttime) / interval;
	}
	int	getXFromTime(time_t t) const {
		return window.getLeft() + getIndexFromTime(t);
	}
	// takes the index and computes the real time timestamp from it
	time_t	getTimeFromIndex(int i) const {
		return starttime + i * interval;
	}
	time_t	getTimekeyFromIndex(int i) const {
		return getTimeFromIndex(i) - offset;
	}

	// color access
	Color	getForeground(void) const { return parent.getForeground(); }
	Color	getBackground(void) const { return parent.getBackground(); }

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
		return getXFromTime(t);
	}

	// accessors for axis
	void	setLeftAxis(const Axis& l) { leftaxis = l; }
	const Axis&	getLeftAxis(void) const { return leftaxis; }
	void	setRightAxis(const Axis& r) { rightaxis = r; }
	const Axis&	getRightAxis(void) const { return rightaxis; }

	// point conversions from Window coordinates (time_t, double)
	// to frame coordinates (int, int)
	Point	getPoint(bool useleftscale, const GraphPoint& gp) const;
	Point	getBottomPoint(time_t t) const;
	Point	getTopPoint(time_t t) const;

	// get a color from a string, or use the foreground for the present
	// image
	Color	getColorFromHexString(const std::string& hexcolorspec) const {
		return parent.getColorFromHexString(hexcolorspec);
	}

	// handle graphing, single points
	void	drawPoint(const Point& p, const Color& color) {
		parent.drawPoint(p, color);
	}
	void	drawPoint(bool useleftscale, const GraphPoint& gp,
		const Color& color) {
		drawPoint(getPoint(useleftscale, gp), color);
	}
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
			linestyle style, bool connected);
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
