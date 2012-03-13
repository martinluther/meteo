/*
 * dloop.h -- loop processing for davis weather stations
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: dloop.h,v 1.1 2001/03/29 18:10:31 afm Exp $
 */
#ifndef _DLOOP_H
#define _DLOOP_H

#include <meteodata.h>

typedef	struct sensorimage {
	double	temperature_inside;
	double	temperature_outside;
	double	wind_speed;
	int	wind_direction;
	double	barometer;
	double	humidity_inside;
	double	humidity_outside;
	double	rain;
} sensorimage_t;

typedef struct loop {
	meteoaccess_t	*m;
	int	(*read)(struct loop *, meteodata_t *md);
} loop_t;

extern loop_t	*dloop_new(meteoaccess_t *m);

#endif /* _DLOOP_H */
