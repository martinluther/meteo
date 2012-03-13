/*
 * Graphics.cc -- draw mete data graphics specified in XML files
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#include <Graphics.h>
#include <mdebug.h>
#include <Query.h>
#include <Dataset.h>
#include <MeteoException.h>

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
	// get the color for the channel we are about to draw
	Color	color = gw->getColorFromHexString(xmlGetAttrString(x, "color"));
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "color is %s", color.getHex().c_str());

	// what side scale do we use
	bool	useleftscale = ("left" == xmlGetAttrString(x, "scale"));
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "using scale on %s",
		useleftscale ? "left" : "right");

	// retrieve the type attribute for this node
	std::string	type = xmlGetAttrString(x, "type");

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
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "drawing curve for %s",
			name.c_str());
		gw->drawLine(useleftscale, ds->getData(name), color, solid);
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

	// if we arrive at this point, then an illegal type specification
	// was found
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "illegal channel type: %s",
		type.c_str());
	throw MeteoException("illegal channel type", type);
}

void	Graphics::drawChannels(const Configuration& conf,
		const std::string& xpath, const Dataset *ds) {
	// retrieve pointers to all nodes matching an xpath
	std::vector<xmlNodePtr>	pnodes = conf.getNodeVector(xpath);

	// go through the vector of node pointers and draw each channel
	std::vector<xmlNodePtr>::const_iterator	i;
	for (i = pnodes.begin(); i != pnodes.end(); i++) {
		drawChannel(*i, gw, ds);
	}
}

typedef std::vector<std::string>	stringvector;

Graphics::Graphics(const Configuration& conf, const std::string& s,
	int interval, time_t end, bool aligncenter,
	const std::string& graphname, bool withdata) : station(s) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "constructing Graphics for station %s,"
		" interval %d, ending at %d, graph name %s", station.c_str(),
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
	Dimension	d(conf, basepath);
	f = new Frame(d);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "frame is at %p", f);

	// set foreground and background
	f->setForeground(conf.getString(basepath + "/@fgcolor", "#000000"));
	f->setBackground(conf.getString(basepath + "/@bgcolor", "#ffffff"));

	// retrieve GraphWindow parameters and create graphwindow
	Rectangle	r(conf, basepath + "/graphwindow");
	gw = new GraphWindow(*f, r);
	if (aligncenter)
		gw->setEndTime(end + interval * (r.getWidth() / 2), interval);
	else
		gw->setEndTime(end, interval);

	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "plotting %d(%d)%d",
		gw->getStartTime(), interval, gw->getEndTime());

	if (withdata) {
		// create query from graph description
		Query	q(interval, gw->getStartTime(), gw->getEndTime());
		stringvector	vs = conf.getStringVector(basepath
					+ "/channels/query/select");
		stringvector	vn = conf.getStringVector(basepath
					+ "/channels/query/select/@name");
		for (int i = 0; i < (int)vn.size(); i++) {
			q.addSelect(vn[i], vs[i]);
		}

		// retrieve data from database
		QueryProcessor	qp(conf, station);
		QueryResult	qr;
		qp.perform(q, qr);

		// compute dependent time series
		Dataset	ds(qr);
		ds.addAlldata(conf, basepath + "/channels/query");

		// get scale and axis definitions
		char	axisbuffer[1024];
		snprintf(axisbuffer, sizeof(axisbuffer),
			"%s/leftaxis[@interval='%d']", basepath.c_str(),
			interval);
		std::string	axispath = std::string(axisbuffer);
		if (!conf.xpathExists(axispath)) {
			axispath = basepath + "/leftaxis";
		}
		gw->setLeftScale(Scale(conf, axispath, ds));
		gw->setLeftAxis(Axis(conf, axispath, gw->getLeftScale()));
		snprintf(axisbuffer, sizeof(axisbuffer),
			"%s/rightaxis[@interval='%d']", basepath.c_str(),
			interval);
		axispath = std::string(axisbuffer);
		if (!conf.xpathExists(axispath)) {
			axispath = basepath + "/rightaxis";
		}
		gw->setRightScale(Scale(conf, axispath, ds));
		gw->setRightAxis(Axis(conf, axispath, gw->getRightScale()));

		// draw graphs
		drawChannels(conf, basepath + "/channels/channel", &ds);
	}

	// draw labels
	if (conf.xpathExists(basepath + "/leftlabel")) {
		f->drawLabel(Label(conf, basepath + "/leftlabel"), true);
	}
	if (conf.xpathExists(basepath + "/rightlabel")) {
		f->drawLabel(Label(conf, basepath + "/rightlabel"), false);
	}

	// draw time axes and grid
	gw->drawTimeGrid();

	// draw value grid and ticks
	gw->drawValueGrid();

	// draw a frame around the graph (this cleans up some artifacts)
	gw->drawFrame();
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

std::string	Graphics::mapString(const std::string& url) const {
	return gw->mapString(url + "&station=" + station);
}

std::string	Graphics::imageTagString(const std::string& filename) const {
	return gw->imageTagString(filename);
}

} /* namespace meteo */