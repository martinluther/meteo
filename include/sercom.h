/*
 * sercom.h -- serial communcations for davis instruments weather station
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _SERCOM_H
#define _SERCOM_H

#include <com.h>

extern meteocom_t	*sercom_new(meteoconf_t *mc, char *dev, int speed);
extern void		sercom_free(meteocom_t *m);

#endif /* _SERCOM_H */
