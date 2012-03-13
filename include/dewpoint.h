/*
 * dewpoint.h -- dew point computation from relative humidity and temperature
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: dewpoint.h,v 1.3 2003/06/08 22:32:10 afm Exp $
 */
#ifndef _DEWPOINT_H
#define _DEWPOINT_H

#include <xmlconf.h>

#ifdef __cplusplus
extern "C" {
#endif

void	find_tempunit(const meteoconf_t *mc, const char *stationname);
double	dewpoint(double humidity, double temp);

#ifdef __cplusplus
}
#endif

#endif /* _DEWPOINT_H */
