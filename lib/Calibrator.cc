/*
 * Calibrator.cc -- implementation of linear transformation
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: Calibrator.cc,v 1.5 2009/01/10 19:00:23 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <Calibrator.h>
#include <mdebug.h>

namespace meteo {

Calibrator::Calibrator(void) {
	topclip = bottomclip = false;
	slope = 1.; y0 = 0.;
}

Calibrator::Calibrator(double m, double y) {
	topclip = bottomclip = false;
	slope = m; y0 = y;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "Calibrator initialized for m = %f "
		"and y0 = %f", slope, y0);
}

Calibrator::Calibrator(double x1, double y1, double x2, double y2) {
	topclip = bottomclip = false;
	slope = (y2 - y1)/(x2 - x1);
	y0 = y1 - slope * x1;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "Calibrator initialized for m = %f "
		"and y0 = %f", slope, y0);
}

Calibrator::~Calibrator(void) {
}

double	Calibrator::operator()(const double x) const {
	double y = slope * x + y0;
	if (topclip) {
		if (y > topcliplevel)
			y = topcliplevel;
	}
	if (bottomclip) {
		if (y < bottomcliplevel)
			y = bottomcliplevel;
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "Calibrator(%f) = %f", x, y);
	return y;
}

} /* namespace meteo */
