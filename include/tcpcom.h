/*
 * tcpcom.h -- tcp based communications. works similarly to serial
 *             interfaces, but simpler, as the serial driver is something
 *             quite complicated
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: tcpcom.h,v 1.1 2002/01/18 23:34:27 afm Exp $
 */
#ifndef _TCPCOM_H
#define _TCPCOM_H

#include <com.h>

extern meteocom_t	*tcpcom_new(char *url);
extern void		tcpcom_free(meteocom_t *m);

#endif /* _TCPCOM_H */
