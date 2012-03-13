/*
 * defaultcolors.h -- default color definitions
 *
 * (c) 2002 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: colors.h,v 1.2 2003/06/06 15:11:05 afm Exp $
 */
#ifndef _COLORS_H
#define _COLORS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ncolor_s {
	int	c[3];
} ncolor_t;

typedef	struct defaultcolors_s {
	ncolor_t	nodata,
			pressure,
			pressure_range,
			temperature,
			temperature_range,
			temperature_dew,
			temperature_inside,
			temperature_inside_range,
			temperature_inside_dew,
			humidity,
			humidity_range,
			humidity_inside,
			humidity_inside_range,
			rain,
			wind,
			gust,
			speed,
			north,
			west,
			south,
			east,
			solar,
			uv,
			bgcolor,
			fgcolor;
} defaultcolors_t;

extern defaultcolors_t defaultcolors;

#ifdef __cplusplus
}
#endif

#endif /* _COLORS_H */
