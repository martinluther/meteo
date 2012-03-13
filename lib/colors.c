/*
 * colors.c -- default values for the various colors, so the can be
 *             referenced more easily
 *
 * (c) 2002 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id$
 */
#include <colors.h>

defaultcolors_t	defaultcolors = {
	/* nodata */
	{ { 204, 204, 204 } },
	/* pressure */
	{ { 102, 102, 255 } },
	/* pressure_range */
	{ { 204, 204, 255 } },
	/* temperature */
	{ { 153, 0, 0 } },
	/* temperature_range */
	{ { 255, 204, 204 } },
	/* temperature_dew */
	{ { 102, 102, 255 } },
	/* temperature_inside */
	{ { 153, 0, 0 } },
	/* temperature_inside_range */
	{ { 255, 204, 204 } },
	/* temperature_inside_dew */
	{ { 102, 102, 255 } },
	/* humidity */
	{ { 0, 0, 255 } },
	/* humidity_range */
	{ { 102, 102, 255 } },
	/* humidity_inside */
	{ { 0, 0, 255 } },
	/* humidity_inside_range */
	{ { 102, 102, 255 } },
	/* rain */
	{ { 0, 0, 255 } },
	/* wind */
	{ { 102, 102, 255 } },
	/* gust */
	{ { 0, 102, 0 } },
	/* speed */
	{ { 0, 255, 0 } },
	/* north */
	{ { 204, 204, 255 } },
	/* west */
	{ { 204,  255, 204 } },
	/* south */
	{ { 255, 255, 204 } },
	/* east */
	{ { 255, 255, 204 } },
	/* solar */
	{ { 204, 153, 0 } },
	/* uv */
	{ { 102, 0, 204 } },
	/* bgcolor */
	{ { 255, 255, 255 } },
	/* fgcolor */
	{ { 0, 0, 0 } }
};
