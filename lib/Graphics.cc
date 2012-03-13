/*
 * Graphics.cc -- draw meteo data graphics specified in XML files
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: Graphics.cc,v 1.19 2004/05/08 20:09:32 afm Exp $
 */
#include <Graphics.h>
#include <Configuration.h>
#include <mdebug.h>
#include <QueryProcessor.h>
#include <Dataset.h>
#include <MeteoException.h>
#include <libxml/tree.h>
#include <SunData.h>
#include <StationInfo.h>

namespace meteo {

std::string	xmlGetAttrString(xmlNodePtr x, const char *attrname) {
	xmlChar	*attr = xmlGetProp(x, (const xmlChar *)attrname);
	if (NULL == attr) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "channel node has no attr "
			"named %s", attrname);
		return std::string();
	}
	std::string	result((char *)attr);
	xmlFree(attr);
	return result;
}

static void	drawChannel(xmlNodePtr x, GraphWindow *gw, const Dataset *ds) {
	// retrieve the type attribute for this node
	std::string	type = xmlGetAttrString(x, "type");

	// get the color for the channel we are about to draw
	std::string	s = xmlGetAttrString(x, "color");
	Color	color;
	if (s == "") {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "no color spec, using "
			"foreground");
		color = gw->getForeground();
	} else {
		color = gw->getColorFromHexString(xmlGetAttrString(x, "color"));
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "color is %s", color.getHex().c_str());

	// what side scale do we use
	bool	useleftscale = ("left" == xmlGetAttrString(x, "scale"));
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "using scale on %s",
		useleftscale ? "left" : "right");

	// handle each of the types
	if (type == "nodata") {
		// draw now data lines whenever there is no data for this 
		// parameter
		std::string	name = xmlGetAttrString(x, "name");
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "drawing nodata for %s",
			name.c_str());
		gw->drawNodata(ds->getData(name), color);
		return;
	}
	if (type == "lowdata") {
		// take the data, remove all the data that lies below the
		// limit, and use it to draw the nodata
		std::string	name = xmlGetAttrString(x, "name");
		std::string	limit = xmlGetAttrString(x, "limit");
		double	l = atof(limit.c_str());

		// remove all the data below the limit
		Tdata	ld = ds->getData(name).limit(l);
		gw->drawNodata(ld, color);
		return;
	}
	if (type == "curve") {
		// draw a continuous curve for this parameter
		std::string	name = xmlGetAttrString(x, "name");
		std::string	connected = xmlGetAttrString(x, "connected");
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "drawing %s curve for %s",
			(connected == "yes") ? "connected" : "disconnected",
			name.c_str());
		gw->drawLine(useleftscale, ds->getData(name), color, solid,
			(connected == "yes"));
		return;
	}
	if (type == "histogram") {
		std::string	name = xmlGetAttrString(x, "name");
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "drawing histogram for %s",
			name.c_str());
		gw->drawHistogram(useleftscale, ds->getData(name), color);
		return;
	}
	if (type == "range") {
		std::string	upper = xmlGetAttrString(x, "nameupper");
		std::string	lower = xmlGetAttrString(x, "namelower");
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "drawing range between "
			"%s and %s", lower.c_str(), upper.c_str());
		gw->drawRange(useleftscale, ds->getData(lower),
			ds->getData(upper), color);
		return;
	}
	// the following is a little bit of a hack, but one can at least
	// argue that the background for wind is additional information like
	// the temperature range. It is tied more to the data than to
	// the background.
	if (type == "windbackground") {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "drawing windbackground");

		// first draw the background colors
		Color	southcolor(xmlGetAttrString(x, "southcolor"));
		Color	northcolor(xmlGetAttrString(x, "northcolor"));
		Color	eastcolor(xmlGetAttrString(x, "eastcolor"));
		Color	westcolor(xmlGetAttrString(x, "westcolor"));
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"windbackground colors: N=%s S=%s W=%s E=%s",
			northcolor.getHex().c_str(),
			southcolor.getHex().c_str(),
			westcolor.getHex().c_str(),
			eastcolor.getHex().c_str());
		gw->drawWindBackground(useleftscale,
			northcolor, southcolor, westcolor, eastcolor);

		// if  we also have the letters attribute set, we also
		// draw the letters in the background color
		std::string	letters = xmlGetAttrString(x, "letters");
		if (letters.length() == 4) {
			gw->drawWindBackgroundLetters(useleftscale, letters);
		}
		return;
	}

	// this is supposed to draw the sunset and sunrise axis color, but
	// we should do this at the and, when everything else has been drawn.
	if (type == "sun") {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "sunrise and sunset delayed");
		return;
	}

	// if we arrive at this point, then an illegal type specification
	// was found
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "illegal channel type: %s",
		type.c_str());
	throw MeteoException("illegal channel type", type);
}

void	Graphics::drawChannels(const std::string& xpath, const Dataset *ds) {
	Configuration	conf;
	// retrieve pointers to all nodes matching an xpath
	std::list<xmlNodePtr>	pnodes = conf.getNodeList(xpath);

	// go through the list of node pointers and draw each channel
	std::list<xmlNodePtr>::const_iterator	i;
	for (i = pnodes.begin(); i != pnodes.end(); i++) {
		drawChannel(*i, gw, ds);
	}
}

Graphics::Graphics(int interval, time_t end, bool aligncenter,
	const std::string& graphname, bool withdata) {
	Configuration	conf;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "constructing Graphics for "
		" interval %d, ending at %d, graph name %s",
		interval, end, graphname.c_str());

	// make sure pointers are properly initialized to 0
	f = NULL;
	gw = NULL;

	// construct the XPath to the graph description in the configuration
	// file
	std::string	basepath = std::string("/meteo/graphs/graph[@name='")
				+ graphname + std::string("']");
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "base path is %s", basepath.c_str());

	// retrieve Frame parameters from configuration, and create Frame
	Dimension	d(basepath);
	f = new Frame(d);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "frame is at %p", f);

	// set foreground and background
	f->setForeground(conf.getString(basepath + "/@fgcolor", "#000000"));
	f->setBackground(conf.getString(basepath + "/@bgcolor", "#ffffff"));

	// we will need a database connection later on
	QueryProcessor	qp(false);

	// retrieve the offset from the configuration
	int	offset = conf.getInt(basepath + "/@offset", 0);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "offset set to %d", offset);

	// retrieve GraphWindow parameters and create graphwindow
	Rectangle	r(basepath + "/graphwindow");
	gw = new GraphWindow(*f, r, offset, interval);
	if (aligncenter)
		gw->setEndTime(end + interval * (r.getWidth() / 2));
	else
		gw->setEndTime(end);

	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "plotting %d(%d)%d",
		gw->getStartTime(), interval, gw->getEndTime());

	if (withdata) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "including data in graph");
		// create query from graph description, this requires that
		// fully qualified field names are inside the <select> tags
		time_t	st = (interval == 60)	? gw->getStartTime()
						: gw->getStartTimekey();
		time_t	et = (interval == 60)	? gw->getEndTime()
						: gw->getEndTimekey();
		Query	q(interval, st, et, gw->getOffset());
		stringlist	vn = conf.getStringList(basepath
					+ "/channels/query/select/@name");
		stringlist::iterator	i;
		for (i = vn.begin(); i != vn.end(); i++) {
			q.addSelect(*i, conf.getString(basepath
				+ "/channels/query/select[@name='"
				+ *i + "']", ""));
		}

		// retrieve data from database
		QueryResult	qr;
		qp.perform(q, qr);

		// compute dependent time series
		Dataset	ds(qr);
		ds.addAlldata(basepath + "/channels/query");

		// get scale and axis definitions
		char	axisbuffer[1024];
		snprintf(axisbuffer, sizeof(axisbuffer),
			"%s/leftaxis[@interval='%d']", basepath.c_str(),
			interval);
		std::string	axispath = std::string(axisbuffer);
		if (!conf.xpathExists(axispath)) {
			axispath = basepath + "/leftaxis";
		}
		gw->setLeftScale(Scale(axispath, ds));
		gw->setLeftAxis(Axis(axispath, gw->getLeftScale()));
		snprintf(axisbuffer, sizeof(axisbuffer),
			"%s/rightaxis[@interval='%d']", basepath.c_str(),
			interval);
		axispath = std::string(axisbuffer);
		if (!conf.xpathExists(axispath)) {
			axispath = basepath + "/rightaxis";
		}
		gw->setRightScale(Scale(axispath, ds));
		gw->setRightAxis(Axis(axispath, gw->getRightScale()));

		// draw graphs
		drawChannels(basepath + "/channels/channel", &ds);
	} else {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "not including data");
	}

	// draw labels
	if (conf.xpathExists(basepath + "/leftlabel")) {
		f->drawLabel(Label(basepath + "/leftlabel"), true);
	}
	if (conf.xpathExists(basepath + "/rightlabel")) {
		f->drawLabel(Label(basepath + "/rightlabel"), false);
	}

	// draw time axes and grid
	gw->drawTimeGrid();

	// draw value grid and ticks
	gw->drawValueGrid();

	// draw a frame around the graph (this cleans up some artifacts)
	gw->drawFrame();

	// now check whether there is a sun channel directive
	drawSun(basepath);
}

Graphics::~Graphics(void) {
	if (NULL != gw)
		delete gw;
	if (NULL != f)
		delete f;
}

void	Graphics::toFile(const std::string& filename) { 
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "writing to file %s",
		filename.c_str());
	f->toFile(filename);
}

std::string	Graphics::mapString(const std::string& url,
			const std::string& station) const {
	return gw->mapString(url + "&station=" + station);
}

std::string	Graphics::imageTagString(const std::string& filename) const {
	return gw->imageTagString(filename);
}

// draw the sunrise/sunset info on the axis
void	Graphics::drawSun(const std::string& basepath) {
	// this does not make sense for the year graphs
	if (gw->getInterval() == 86400) {
		return;
	}

	// create a new SunData object
	Configuration	conf;
	std::string	station = conf.getString(basepath
		+ "/channels/channel[@type='sun']/@station", "");
	if (station == "") {
		// if station not found, we assume it is not necessary to draw
		// the sun stuff
		return;
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "sunrise/sunset station: %s",
		station.c_str());

	// get the offset
	StationInfo	s(station);
	int	offset = s.getOffset();

	// find the color
	Color	color = gw->getColorFromHexString(conf.getString(basepath
		+ "/channels/channel[@type='sun']/@color", "#ff0000"));

	// create the SunData object
	SunData	sd(station, offset);

	// find out whether this station should use the database or 
	// compute sun position itself
	std::string	method = conf.getString("/meteo/station[@name='"
		+ station + "']/sun/@method", "database");
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "method: %s", method.c_str());
	if (method != "database") {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "switching to computed sun");
		sd.setCompute(true);
	}

	// create the list of points that are during the day (sun above horizon)
	std::vector<time_t>	r = sd(gw->getInterval(), gw->getStartTimekey(),
		gw->getEndTimekey());
	for (std::vector<time_t>::iterator i = r.begin(); i != r.end(); i++) {
		time_t	t = *i - offset;
		gw->drawPoint(gw->getBottomPoint(t), color);
	}
}

} /* namespace meteo */
