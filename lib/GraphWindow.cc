/*
 * GraphWindow.cc -- implementation of graphing functions
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#include <GraphWindow.h>
#include <mdebug.h>
#include <gdfonts.h>
#include <Dataset.h>
#include <MeteoException.h>
#include <algo.h>

namespace meteo {


//////////////////////////////////////////////////////////////////////
// Scale methods
//////////////////////////////////////////////////////////////////////

// Scale::Scale -- construct Scale object from the configuration based on
//                 an xpath
Scale::Scale(const Configuration& conf, const std::string& xpath,
	const Dataset& ds) {
	double		min, max, max2, min2, r;
	std::string	minname, maxname;
	// get the type from the configuration file
	std::string	type = conf.getString(xpath + "/@type", "static");

	// retrieve basic parameters
	min2 = conf.getDouble(xpath + "/@min", 0.);
	max2 = conf.getDouble(xpath + "/@max", 1.);
	r = max2 - min2;
	a = 1/r;

	// the attributes min and max make up the scale (in the static case)
	if (type == "static") {
		b = min2;
		return;
	}

	// the dynamic range adapts to the data to be displayed
	maxname = conf.getString(xpath + "/@maxname", "none");
	if (maxname == "none") {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "no maxname specified");
		throw MeteoException("no maxname specified", xpath);
	}
	max = ds.getData(maxname).max();
	minname = conf.getString(xpath + "/@minname", "none");
	if (minname == "none") {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "no minname specified");
		throw MeteoException("no minname specified", xpath);
	}
	min = ds.getData(minname).min();
	if (type == "dynamic") {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "dynamic range %f - %f",
			min, max);
		b = min;
		a = 1/(max - min);
		return;
	}

	// a range type scale keeps the same
	if (type == "toprange") {
		// max > max2: use the max data as the top border
		if (max > max2) {
			b = max - r; return;
		}
		// data becomes smaller than we thought in the conf file
		if (min < min2) {
			if (min + r < max) {
				// data does not fit in range
				b = max - r; return;
			} else {
				// data fits in range, go no lower than min
				b = min;
			}
		}
		b = min2; return;
	}
	if (type == "bottomrange") {
		// min < min2: use the min data as the top border
		if (min < min2) {
			b = min; return;
		}
		// data becomes larger than we thought in the conf file
		if (max > max2) {
			if (max - r > min) {
				// data does not fit in range, follow min
				b = min; return;
			} else {
				// data fits in range, go no higher than max2
				b = max - r;
			}
		}
		b = min2; return;
	}
}

//////////////////////////////////////////////////////////////////////
// Axis methods
//////////////////////////////////////////////////////////////////////

// Axis::Axis -- construct a Axis object from the configuration
//                     based on an Xpath
#define	MINORIGIN	-1000000000.
Axis::Axis(const Configuration& conf, const std::string& xpath,
	const Scale& sc) {
	// the attributes first, last, step and format and the flags ticks
	// and gridlines make up the axis obj
	std::string	type = conf.getString(xpath + "/@type", "static");
	format = conf.getString(xpath + "/@format", "%f");
	first = conf.getDouble(xpath + "/@first", 0.);
	last = conf.getDouble(xpath + "/@last", 0.);
	step = conf.getDouble(xpath + "/@step", 1.);
	ticks = conf.getBool(xpath + "/@ticks", false);
	gridlines = conf.getBool(xpath + "/@gridlines", false);

	// for the static/default Axis definition, this is all we need
	if (type == "static") {
		return;
	}

	// compute a suitable origin (if not set explicitely in the config file
	double	origin = conf.getDouble(xpath + "/@origin", MINORIGIN);
	if (origin == MINORIGIN) {
		origin = first - floor(first/step) * step;
	}

	// we want at most maxtickcount ticks, but the step should be
	// of the form step * 10^n * x, where x \in {1,2,5}
	if (type == "dynamic") {
		// compute a suitable step, i.e. the number of ticks should
		// be as large as possible with the current ticklimit
		int	maxtickcount = conf.getInt(xpath + "/@maxtickcount",
						10);
		double	stepbase = pow(10.,
				floor(log10(sc.range()/(step * maxtickcount))));
		if ((5 * stepbase) >= (sc.range()/(step * maxtickcount))) {
			step = 5 * stepbase * step;
		}
		if ((2 * stepbase) >= (sc.range()/(step * maxtickcount))) {
			step = 2 * stepbase * step;
		}
		if (stepbase >= (sc.range()/(step * maxtickcount))) {
			step *= stepbase;
		}
	}

	// compute first and last anew, they must be multiples of the step
	last = origin + step * floor((sc.getMax() - origin)/step);
	first = origin - step * floor((origin - sc.getMin())/step);
}

//////////////////////////////////////////////////////////////////////
// MapArea methods
//////////////////////////////////////////////////////////////////////

// turn a map area into a string representation of the map
// @baseurl  	must contain language, station name and level
std::string	MapArea::getStringForm(const std::string& baseurl) const {
	std::string	result;
	char	buffer[1024];
	snprintf(buffer, sizeof(buffer), "<area coords=\"%d,%d,%d,%d\" ",
		area.getLeft(), area.getLow(), area.getRight(), area.getHigh());
	result = std::string(buffer);
	snprintf(buffer, sizeof(buffer), "alt=\"%s\" ",
		ti.getString().c_str());
	result += buffer;
	snprintf(buffer, sizeof(buffer), "href=\"%s&label=%s\" />\n",
		baseurl.c_str(), ti.getString().c_str());
	result += buffer;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "area string is %s", result.c_str());
	return result;
}

//////////////////////////////////////////////////////////////////////
// ImageMap methods
//////////////////////////////////////////////////////////////////////
class	DisplayMap {
	std::string	mapstring;
	std::string	baseurl;
public:
	DisplayMap(const std::string& url) { baseurl = url; }
	~DisplayMap(void) { }
	void	operator()(const MapArea& m) {
		mapstring += m.getStringForm(baseurl);
	}
	std::string	getStringForm(void) const { return mapstring; }
};

std::string	ImageMap::getStringForm(const std::string& url) const {
	std::string	result;
	result = "<map name=\"" + level.getLevelString() + "map\">\n";
	DisplayMap d = for_each(areas.begin(), areas.end(), DisplayMap(url));
	result += d.getStringForm();
	result += "</map>\n";
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "map string: %s", result.c_str());
	return result;
}
std::string	ImageMap::getImageTag(const std::string& filename) const {
	std::string	result = "<img src=\"";
	result += filename + "\" ";
	result += "usemap=\"#" + level.getLevelString() + "map\" ";
	result += "border=\"0\" ";
	return result + " />\n";
}

//////////////////////////////////////////////////////////////////////
// GraphWindow methods
//////////////////////////////////////////////////////////////////////

// drawLine -- draw a line joining the data points of data in the specified
//             color. If a datapoint is missing, interrupt the graph
void	GraphWindow::drawLine(bool useleftscale, const Tdata& data,
		const Color& color, linestyle style) {
	GraphPoint	previous;
	bool		haveprevious = false;
	tdata_t::const_iterator	j;

	for (int i = 0; i < getWidth(); i++) {
		time_t	timeindex = getTimeFromIndex(i);
		j = data.find(timeindex);
		if (j == data.end()) {
			haveprevious = false;
		} else {
			GraphPoint	current(j->first, j->second);
			// draw a line if the previous point is set
			if (haveprevious) {
				drawLine(useleftscale, previous, current,
					color, style);
			}
			previous = current;
			haveprevious = true;
		}
	}
}

// drawHistogram -- draw vertical lines from the data point to the bottom
//                  of the graph in the specified color (used for solar
//                  radiation and wind)
void	GraphWindow::drawHistogram(bool useleftscale, const Tdata& data,
		const Color& color) {
	tdata_t::const_iterator	i;
	for (i = data.begin(); i != data.end(); i++) {
		time_t	timeindex = i->first;
		parent.drawLine(getBottomPoint(timeindex),
			getPoint(useleftscale,
				GraphPoint(timeindex, i->second)),
			color, solid);
	}
}

// drawRange -- draw the range between the two data sets lower and upper in
//              the specified color (used for temperature, humidity and
//              pressure graphs)
void	GraphWindow::drawRange(bool useleftscale, const Tdata& lower,
		const Tdata& upper, const Color& color) {
	tdata_t::const_iterator	i, j;
	for (i = lower.begin(); i != lower.end(); i++) {
		time_t	timeindex = i->first;
		j = upper.find(timeindex);
		if (j != upper.end()) {
			drawLine(useleftscale,
				GraphPoint(i->first, i->second),
				GraphPoint(j->first, j->second), color, solid);
		}
	}
}

// drawNodata -- for each time point where we don't have any data, write
//               a vertical line over the graph with the color specified
//               (usually gray)
void	GraphWindow::drawNodata(const Tdata& data, const Color& color) {
	for (int i = 0; i <= getWidth(); i++) {
		time_t	t = getTimeFromIndex(i);
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "nodata at %d->%d", i, t);
		if (data.end() == data.find(t)) {
			parent.drawLine(getTopPoint(t),
				getBottomPoint(t), color, solid);
		}
	}
}

// drawWindBackground -- based on a scale and for colors, draw background
//                       for the wind graph the illustrates somewhat more
//                       easily the wind direction
void	GraphWindow::drawWindBackground(bool useleftscale,
		const Color& northcolor, const Color& southcolor,
		const Color& westcolor, const Color& eastcolor) {
	// wind from south
	drawRectangle(useleftscale,
		GraphPoint(starttime, 0.), GraphPoint(endtime, 45.),
		southcolor);
	drawRectangle(useleftscale,
		GraphPoint(starttime, 315.), GraphPoint(endtime, 360.),
		southcolor);
	// wind from north
	drawRectangle(useleftscale,
		GraphPoint(starttime, 135.), GraphPoint(endtime, 225.),
		northcolor);
	// wind from east
	drawRectangle(useleftscale,
		GraphPoint(starttime, 225.), GraphPoint(endtime, 315.),
		eastcolor);
	// wind from west
	drawRectangle(useleftscale,
		GraphPoint(starttime, 45.), GraphPoint(endtime, 135.),
		westcolor);
}

void	GraphWindow::drawWindBackgroundLetters(bool useleftscale,
		const std::string& letters) {
	int	step = (3 * gdFontSmall->w * interval) / 2;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "drawing wind background letters %s",
		letters.c_str());

	// draw letters between starttime + step and endtime - step
	for (time_t t = starttime + step; t < endtime - step; t += 2 * step) {
		// draw a letter for each angle value, draw angles 0. and 360.
		// so that half that letter gets displayed in any case
		for (int i = 0; i <= 4; i++) {
			double azi = i * 90.;
			drawLetter(useleftscale, letters.data()[i % 4],
				GraphPoint(t, azi), parent.getBackground());
		}
	}
}

static bool	startOfHalfhour(time_t t, int interval, struct tm *tmp) {
	struct tm	*lt;
	if (NULL == tmp) {
		if (interval < 3600)
			t -= t % interval;
		else
			t -= t % 3600;
		lt = localtime(&t);
	} else
		lt = tmp;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "startOfHour(%d): tm_hour = %d, "
		"tm_min = %d, tm_sec = %d", t, lt->tm_hour, lt->tm_min,
		lt->tm_sec);
	return (((lt->tm_min == 0) || (lt->tm_min == 30)) && (lt->tm_sec == 0));
}

static bool	startOfHour(time_t t, int interval, struct tm *tmp) {
	struct tm	*lt;
	if (NULL == tmp) {
		if (interval < 3600)
			t -= t % interval;
		else
			t -= t % 3600;
		lt = localtime(&t);
	} else
		lt = tmp;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "startOfHour(%d): tm_hour = %d, "
		"tm_min = %d, tm_sec = %d", t, lt->tm_hour, lt->tm_min,
		lt->tm_sec);
	return ((lt->tm_min == 0) && (lt->tm_sec == 0));
}

static bool	startOfEvenHour(time_t t, int interval, struct tm *tmp) {
	struct tm	*lt;
	if (NULL == tmp) {
		t -= t % interval;
		lt = localtime(&t);
	} else
		lt = tmp;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "startOfEvenHour(%d): tm_hour = %d, "
		"tm_min = %d, tm_sec = %d", t, lt->tm_hour, lt->tm_min,
		lt->tm_sec);
	return (((lt->tm_hour % 2) == 0) && (lt->tm_min == 0) && (lt->tm_sec == 0));
}

static bool	startOfDay(time_t t, int interval, struct tm *tmp) {
	struct tm	*lt;
	if (NULL == tmp) {
		t -= t % interval;
		lt = localtime(&t);
	} else
		lt = tmp;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "startOfDay(%d): tm_hour = %d, "
		"tm_min = %d", t, lt->tm_hour, lt->tm_min);
	return ((lt->tm_hour == 0) && (lt->tm_min == 0));
}

static bool	middleOfDay(time_t t, int interval, struct tm *tmp) {
	struct tm	*lt;
	if (NULL == tmp) {
		t -= t % interval;
		lt = localtime(&t);
	} else
		lt = tmp;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "middleOfDay: tm_hour = %d, "
		"tm_min = %d", lt->tm_hour, lt->tm_min);
	return ((lt->tm_hour == 12) && (lt->tm_min == 0));
}

// weeks start on sunday
static bool	startOfWeek(time_t t, int interval, struct tm *tmp) {
	struct tm	*lt;
	if (NULL == tmp) {
		t -= t % interval;
		lt = localtime(&t);
	} else
		lt = tmp;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "startOfWeek(%d): tm_wday = %d, "
		"tm_hour = %d, tm_min = %d", t, lt->tm_wday, lt->tm_hour,
		lt->tm_min);
	return (((lt->tm_hour == 0) || (lt->tm_hour == 1)) && (lt->tm_wday == 0));
}

static bool	middleOfWeek(time_t t, int interval, struct tm *tmp) {
	struct tm	*lt;
	if (NULL == tmp) {
		t -= t % interval;
		lt = localtime(&t);
	} else
		lt = tmp;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "middleOfWeek(%d): tm_wday = %d, "
		"tm_hour = %d, tm_min = %d", t, lt->tm_wday, lt->tm_hour,
		lt->tm_min);
	return (((lt->tm_hour == 12) || (lt->tm_hour == 13)) && (lt->tm_wday == 3));
}

static bool	startOfMonth(time_t t, int interval, struct tm *tmp) {
	struct tm	*lt;
	if (NULL == tmp) {
		t -= t % interval;
		lt = localtime(&t);
	} else
		lt = tmp;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "startOfMonth(%d): tm_wday = %d, "
		"tm_hour = %d, tm_min = %d", t, lt->tm_mday, lt->tm_hour,
		lt->tm_min);
	return (lt->tm_mday == 1);
}

static bool	middleOfMonth(time_t t, int interval, struct tm *tmp) {
	struct tm	*lt;
	if (NULL == tmp) {
		t -= t % interval;
		lt = localtime(&t);
	} else
		lt = tmp;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "middleOfMonth(%d): tm_mday = %d, "
		"tm_hour = %d, tm_min = %d", t, lt->tm_mday, lt->tm_hour,
		lt->tm_min);
	return (lt->tm_mday == 15);
}

MapArea	GraphWindow::getArea(time_t start, time_t end) const {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "adding area %d - %d", start, end);
	// compute dimensions for the rectangle
	Point	ll(getX(start), window.getLow());
	Point	ur(getX(end), window.getHigh());
	Rectangle r(ll, ur);

	// compute mid time for this area
	Level	l(interval);
	Timelabel	tl((start + end)/2, l.down());

	return MapArea(r, tl.getString(), start);
}

// drawTimeGrid -- draw the time grid for this resolution, note that time 
//                 is drawn in local time, while the time_t arguments are
//                 UTC, so we cannot just use draw a tick whenever the remainder
//                 of t divided by a suitable multible of the interval is 0
void	GraphWindow::drawVerticalLine(time_t t, linestyle style) {
	parent.drawLine(Point(getXFromTime(t), window.getHigh()),
		Point(getXFromTime(t), window.getLow()),
		parent.getForeground(), style);
}
void	GraphWindow::drawTimeTick(time_t t) {
	parent.drawLine(Point(getXFromTime(t), window.getLow() + 2),
		Point(getXFromTime(t), window.getLow() - 2),
		parent.getForeground(), solid);
}
void	GraphWindow::drawTimeLabel(time_t t, const std::string& label) {
	int	x;
	// find the length of the string and compute the width
	x = (int)(getXFromTime(t) - (0.5 * gdFontSmall->w * label.length()));
	Point	start(x, window.getLow() - 5);

	// draw the label
	parent.drawText(label, start, parent.getForeground(), true);
}
void	GraphWindow::drawTimeGrid(void) {
	char	label[32];
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "creating time grid");
	time_t	areastart = starttime;
	for (time_t t = starttime; t <= endtime; t += interval) {
		struct tm	*tmp = localtime(&t);
		struct tm	lt;
		memcpy(&lt, tmp, sizeof(lt));
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "drawTimeGrid time %d", t);
		switch (interval) {
		case 60:
			if (startOfHalfhour(t, interval, &lt)) {
				drawVerticalLine(t, dotted);
				drawTimeTick(t);
			}
			if (startOfHour(t, interval, &lt)) {
				strftime(label, sizeof(label), "%H", &lt);
				drawTimeLabel(t, label);
			}
			break;
		case 300:
			if (startOfHour(t, interval, &lt)) {
				drawVerticalLine(t, dotted);
				drawTimeTick(t);
			}
			if (startOfEvenHour(t, interval, &lt)) {
				strftime(label, sizeof(label), "%H", &lt);
				drawTimeLabel(t, label);
			}
			break;
		case 1800:
			if (startOfDay(t, interval, &lt)) {
				drawVerticalLine(t, dotted);
				drawTimeTick(t);
				imap.addArea(getArea(areastart, t));
				areastart = t;
			}
			if (middleOfDay(t, interval, &lt)) {
				strftime(label, sizeof(label), "%a", &lt);
				drawTimeLabel(t, label);
			}
			break;
		case 7200:
			if (startOfWeek(t, interval, &lt)) {
				drawVerticalLine(t, dotted);
				drawTimeTick(t);
				imap.addArea(getArea(areastart, t));
				areastart = t;
			}
			if (middleOfWeek(t, interval, &lt)) {
				strftime(label, sizeof(label), "week %V", &lt);
				drawTimeLabel(t, label);
			}
			break;
		case 86400:
			if (startOfMonth(t, interval, &lt)) {
				drawVerticalLine(t, dotted);
				drawTimeTick(t);
				imap.addArea(getArea(areastart, t));
				areastart = t;
			}
			if (middleOfMonth(t, interval, &lt)) {
				strftime(label, sizeof(label), "%b", &lt);
				drawTimeLabel(t, label);
			}
			break;
		}
	}
	// add image map area for last displayed data
	if (interval > 300)
		imap.addArea(getArea(areastart, endtime));
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "time grid complete");
}

// drawValueGrid -- draw the horizontal grid lines, the ticks and the
//                  value labels
void	GraphWindow::drawValueGridlines(bool useleftscale) {
	// first find out which axis object to use for the grid
	Axis	ax = (useleftscale) ? leftaxis : rightaxis;

	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "drawing value grid from %f to %f, "
		"step %f", ax.getFirst(), ax.getLast(), ax.getStep());

	// draw the dotted lines and tick marks
	for (double gridvalue = ax.getFirst(); gridvalue <= ax.getLast();
		gridvalue += ax.getStep()) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "grid value for %f",
			gridvalue);
		int	y = getY(useleftscale, gridvalue);

		// a dotted line
		parent.drawLine(Point(window.getLeft(), y),
			Point(window.getRight(), y), parent.getForeground(),
			dotted);
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "done with value grid");
}

void	GraphWindow::drawValueTicks(bool useleftscale) {
	// first find out which axis object to use for the grid
	Axis	ax = (useleftscale) ? leftaxis : rightaxis;

	// draw the dotted lines and tick marks
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "drawing ticks/labels from %f to %f, "
		"step %f, label format %s", ax.getFirst(), ax.getLast(),
		ax.getStep(), ax.getFormat().c_str());
	for (double gridvalue = ax.getFirst(); gridvalue <= ax.getLast();
		gridvalue += ax.getStep()) {
		// select the right points (left/right) for ticks
		int	x, y = getY(useleftscale, gridvalue);
		if (useleftscale) {
			x = window.getLeft() - 1;
		} else {
			x = window.getRight() + 1;
		}

		// the tick mark
		parent.drawLine(Point(x - 2, y), Point(x + 2, y),
			parent.getForeground(), solid);

		// place the tick label (place it half a character height
		// below the tick line and at the width of the tick label string
		// to the left
		char	label[32];
		snprintf(label, sizeof(label), ax.getFormat().c_str(),
			gridvalue);
		y += (int)(0.5 * gdFontSmall->h);
		if (useleftscale) {
			int	w = gdFontSmall->w * strlen(label);
			parent.drawText(std::string(label),
				Point(x - w - 3, y), parent.getForeground(),
				true);
		} else {
			parent.drawText(std::string(label),
				Point(x + 3, y), parent.getForeground(),
				true);
		}
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "done with value ticks");
}

void	GraphWindow::drawValueGrid(void) {
	if (leftaxis.getGridlines())
		drawValueGridlines(true);
	if (leftaxis.getTicks())
		drawValueTicks(true);
	if (rightaxis.getGridlines())
		drawValueGridlines(false);
	if (rightaxis.getTicks())
		drawValueTicks(false);
}

// draw a frame around the graph
void	GraphWindow::drawFrame(void) {
	parent.drawLine(Point(window.getLeft() - 1, window.getLow()),
		Point(window.getRight() + 1, window.getLow()),
		parent.getForeground(), solid);
	parent.drawLine(Point(window.getLeft() - 1, window.getHigh()),
		Point(window.getRight() + 1, window.getHigh()),
		parent.getForeground(), solid);
	parent.drawLine(Point(window.getLeft() - 1, window.getLow()),
		Point(window.getLeft() - 1, window.getHigh()),
		parent.getForeground(), solid);
	parent.drawLine(Point(window.getRight() + 1, window.getLow()),
		Point(window.getRight() + 1, window.getHigh()),
		parent.getForeground(), solid);
}

// return string form of the image map
std::string	GraphWindow::mapString(const std::string& url) const {
	return imap.getStringForm(url);
}

std::string	GraphWindow::imageTagString(const std::string& filename) const {
	return imap.getImageTag(filename);
}

} /* namespace meteo */
