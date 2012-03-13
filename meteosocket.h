/*
 * meteosocket.h -- create a socket, format messages sent to it, or parse
 *                  messages received on it
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: meteosocket.h,v 1.1 2001/12/24 14:39:46 afm Exp $
 */
#ifndef _METEOSOCKET_H
#define _METEOSOCKET_H

#include <meteodata.h>

#define	METEO_SERVER	0
#define	METEO_CLIENT	1

extern int	meteosocket(char *socketpath, int mode);
extern int	meteorecv(int s, meteodata_t *md, char **station);
extern int	meteosend(int s, meteodata_t *md, char *station);

#endif /* _METEOSOCKET_H */
