/*
 * Frame.cc -- framework for drawing graphs
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#include <Frame.h>
#include <mdebug.h>
#include <MeteoException.h>
#include <Configuration.h>
#include <gd.h>
#include <gdfonts.h>
#include <map>
#include <mdebug.h>
#include <errno.h>
#include <string.h>
#include <algo.h>

namespace meteo {

//////////////////////////////////////////////////////////////////////
// Dimension class methods
Dimension::Dimension(const Configuration& conf, const std::string& xpath) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "constructing Dimension from %s",
		xpath.c_str());
	height = conf.getInt(xpath + "/@height", 144);
	width = conf.getInt(xpath + "/@width", 500);
}

//////////////////////////////////////////////////////////////////////
// Rectangle class methods
Rectangle::Rectangle(const Configuration& conf, const std::string& xpath) {
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

//////////////////////////////////////////////////////////////////////
// Color class methods

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

int	Color::getValue(void) const {
	return gdTrueColorAlpha(red, green, blue, alpha);
}

std::string	Color::getHex(void) const {
	char	buffer[80];
	snprintf(buffer, sizeof(buffer), "#%02x%02x%02x", red, green, blue);
	if (alpha > 0) {
		snprintf(buffer + 7, sizeof(buffer) - 7, "%02x", alpha);
	}
	return std::string(buffer);
}

//////////////////////////////////////////////////////////////////////
// Label class methods
Label::Label(const Configuration& conf, const std::string& xpath) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "constructing label from %s",
		xpath.c_str());
	std::string	alignment = conf.getString(xpath + "/@align", "center");
	if ("center" == alignment)
		align = center;
	if ("top" == alignment)
		align = top;
	if ("bottom" == alignment)
		align = bottom;
	text = conf.getString(xpath, "label");
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "label text: %s", text.c_str());
}

//////////////////////////////////////////////////////////////////////
// Frame internal class and methods
class	frame_internals {
	Dimension		dimension;
	gdImagePtr		gd;
	int			ls[6]; 	// linestyle
	int			refcount;
public:
	// construction
	frame_internals(const Dimension& dim);
	~frame_internals(void);

	// coordinate transformation
	int	getX(const Point& p) const { return p.getX(); }
	int	getY(const Point& p) const {
		return dimension.getHeight() - p.getY() - 1;
	}

	// color table access
	int	getColorIndex(const Color& color);
	int	addColor(const Color& color);
	int	setBackground(const Color& color);

	// drawing and writing
	void	setLinestyle(const Color& c, linestyle style);
	void	drawLine(const Point& p1, const Point& p2, const Color& c,
			linestyle style);
	void	drawRectangle(const Rectangle& rectangle, const Color& c);
	void	drawText(const std::string& text, const Point& p1,
		const Color& c, bool horizontal);
	void	drawLetter(const char c, const Point& center, const Color& c);

	// write to file
	void	toFile(const std::string& filename) const;

	// declare Frame as a friend, so it can access the internal state
	friend class	Frame;
};

frame_internals::frame_internals(const Dimension& dim) : dimension(dim) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "creating frame %d/%d",
		dim.getWidth(), dim.getHeight());
	gd = gdImageCreateTrueColor(dimension.getWidth(),
		dimension.getHeight());
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "frame created");
}

frame_internals::~frame_internals(void) {
	gdImageDestroy(gd);
}

int	frame_internals::getColorIndex(const Color& color) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "finding color index for %s",
		color.getHex().c_str());
	return color.getValue();
}

int	frame_internals::addColor(const Color& color) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "adding color %s",
		color.getHex().c_str());
	return color.getValue();
}

int	frame_internals::setBackground(const Color& color) {
	int	ci = getColorIndex(color);
	gdImageFilledRectangle(gd, 0, 0,
		dimension.getWidth() - 1, dimension.getHeight() - 1, ci);
	return ci;
}

void	frame_internals::setLinestyle(const Color& color, linestyle style) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "setting linestyle %d with color %s",
		style, color.getHex().c_str());

	// first retrieve the color
	int	ci = color.getValue();

	// the prepare the line style array
	switch (style) {
	case solid:
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "solid color %d", ci);
		break;
	case dotted:
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "setting dotted color %d", ci);
		ls[0] = ci; ls[1] = ls[2] = gdTransparent;
		gdImageSetStyle(gd, ls, 3);
		break;
	case dashed:
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "setting dashed color %d", ci);
		ls[0] = ls[1] = ls[2] = ls[3] = ci;
		ls[4] = ls[5] = gdTransparent;
		gdImageSetStyle(gd, ls, 6);
		break;
	}

	// XXX set the style for the next drawing operation
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "drawing style set");
}

void	frame_internals::drawLine(const Point& p1, const Point& p2,
		const Color& color, linestyle style) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "drawing line (%d, %d) - (%d, %d)\n",
		p1.getX(), p1.getY(), p2.getX(), p2.getY());
	// first find the index belonging to the color
	int	colorindex = getColorIndex(color);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "colorindex is %d", colorindex);

	// convert the point coordinates into internal coordinates
	int	colorconst;
	
	if (style == solid)
		colorconst = colorindex;
	else {
		setLinestyle(color, style);
		colorconst = gdStyled;
	}
	gdImageLine(gd, getX(p1), getY(p1),
			getX(p2), getY(p2), colorconst);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "line complete");
}

void	frame_internals::drawText(const std::string& text, const Point& p,
		const Color& color, bool horizontal) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "writing text '%s' at (%d, %d)",
		text.c_str(), p.getX(), p.getY());
	// find the color index belonging to this color
	int	colorindex = getColorIndex(color);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "colorindex is %d", colorindex);

	// set the font
	gdFontPtr	f = gdFontSmall;

	// draw the string
	if (horizontal) {
		gdImageString(gd, f, getX(p), getY(p),
			(unsigned char *)text.c_str(), colorindex);
	} else {
		gdImageStringUp(gd, f, getX(p), getY(p),
			(unsigned char *)text.c_str(), colorindex);
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "string displayed");
}

void	frame_internals::drawLetter(const char c, const Point& center,
		const Color& color) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "writing letter '%c' at (%d, %d)", c,
		center.getX(), center.getY());
	// find the color index belonging to this color
	int	colorindex = getColorIndex(color);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "colorindex is %d", colorindex);

	// set the font
	gdFontPtr	f = gdFontSmall;

	// build a string containing exactly the letter
	unsigned char	str[2];
	str[0] = c; str[1] = '\0';

	// compute the upper left corner of the character that is to 
	// contain the letter
	int	x, y;
	x = getX(center); y = getY(center);
	x -= f->w/2;
	y -= f->h/2;

	// draw the letter
	gdImageString(gd, f, x, y, str, colorindex);

	// inform the log
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "character displayed");
}

// drawRectangle -- draw a filled rectangle. 
//
// note the somewhat contorted way to specify the rectangle, this has to do
// with the fact that the GD library wants an upper left and a lower right
// point, while we have a lower left und uppert right point. So to construct
// the point GD wants, we have to mix the two.
void	frame_internals::drawRectangle(const Rectangle& rectangle,
	const Color& color) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "drawing rectangle (%d,%d), (%d,%d)",
		getX(rectangle.getLowerLeft()),
		getY(rectangle.getUpperRight()),
		getX(rectangle.getUpperRight()),
		getY(rectangle.getLowerLeft()));

	// find the color index
	int	colorindex = getColorIndex(color);

	// draw a rectangle
	gdImageFilledRectangle(gd,
		getX(rectangle.getLowerLeft()),
		getY(rectangle.getUpperRight()),
		getX(rectangle.getUpperRight()),
		getY(rectangle.getLowerLeft()),
		colorindex);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "drawing rectangle complete");
}

void	frame_internals::toFile(const std::string& filename) const {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "writing image to file %s",
		filename.c_str());
	FILE	*file = fopen(filename.c_str(), "wb");
	if (NULL == file) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0,
			"cannot open output file '%s': %s (%d)",
			filename.c_str(), strerror(errno), errno);
		throw MeteoException(std::string("cannot open output file")
			+ filename, strerror(errno));
	}
	gdImagePng(gd, file);
	fclose(file);
}

//////////////////////////////////////////////////////////////////////
// Frame methods
Frame::Frame(const Dimension& o) : outer(o) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "construct new Frame from dimension");
	fi = NULL;
	setupInternals();
	fi->refcount = 1;
}

Frame::Frame(const Frame& other) : outer(other.outer) {
	fi = other.fi;
	fi->refcount++;
	foreground = other.foreground;
	background = other.background;
}

Frame::~Frame(void) {
	if (!fi->refcount--) {
		delete fi;
		fi = NULL;
	}
}

Frame&	Frame::operator=(const Frame& other) {
	// canonical 
	other.fi->refcount++;
	if (--fi->refcount == 0) delete fi;
	fi = other.fi;
	outer = other.outer;
	foreground = other.foreground;
	background = other.background;
	return *this;
}

void	Frame::setupInternals(void) {
	if (fi == NULL) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "setting up Frame internals");
		fi = new frame_internals(outer);
	}
}

void	Frame::setForeground(const Color& c) {
	foreground = c;
}

void	Frame::setForeground(const std::string& c) {
	foreground = Color(c);
	setForeground(foreground);
}

void	Frame::setBackground(const Color& c) {
	fi->setBackground(c);
	background = c;
}

void	Frame::setBackground(const std::string& c) {
	background = Color(c);
	setBackground(background);
}

Color	Frame::getColorFromHexString(const std::string& c) const {
	// parse the color string, if that fails, use the foreground color
	try {
		Color	color(c);
		return color;
	} catch (...) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "cannot parse color '%s', "
			"using foreground", c.c_str());
	}
	return foreground;
}

void	Frame::drawLine(const Point& p1, const Point& p2, const Color& color,
		linestyle style) {
	setupInternals();
	fi->drawLine(p1, p2, color, style);
}

void	Frame::drawRectangle(const Rectangle& r, const Color& color) {
	setupInternals();
	fi->drawRectangle(r, color);
}

void	Frame::drawText(const std::string& t, const Point& start,
	const Color& color, bool horizontal) {
	setupInternals();
	fi->drawText(t, start, color, horizontal);
}

void	Frame::drawLetter(const char c, const Point& start,
	const Color& color) {
	setupInternals();
	fi->drawLetter(c, start, color);
}

void	Frame::drawLabel(const std::string& text, bool left, labelalign_t a) {
	int	x, y;
	if (left) {
		x = 0;
	} else {
		x = outer.getWidth() - gdFontSmall->h - 2;
	}
	switch (a) {
	case top:
		y = (int)(outer.getHeight()
			- gdFontSmall->w * text.length() - 2);
		break;
	case center:
		y = (int)(outer.getHeight()/2
			- (0.5 * gdFontSmall->w * text.length()) - 1);
		break;
	case bottom:
		y = 2;
		break;
	}
	drawText(text, Point(x, y), foreground, false);
}
void	Frame::drawLabel(const Label& label, bool left) {
	drawLabel(label.getText(), left, label.getAlign());
}

void	Frame::toFile(const std::string& filename) {
	setupInternals();
	fi->toFile(filename);
}

} /* namespace meteo */
