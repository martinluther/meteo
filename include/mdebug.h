/*
 * mdebug.h -- meteo debugging subsystem
 *
 * (c) 2002 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: mdebug.h,v 1.1 2002/01/27 21:01:42 afm Exp $
 */
#ifndef _MDEBUG_H
#define _MDEBUG_H

#include <syslog.h>
#include <stdarg.h>
#include <stdio.h>

#define	MDEBUG_NOFILELINE	1
#define	MDEBUG_ERRNO		2
#define	MDEBUG_LOG		__FILE__, __LINE__

extern int	debug;
extern int	mdebug_setup(const char *ident, const char *logurl);
extern int	mdebug_setup_file(const char *ident, FILE *lgf);
extern void	vmdebug(int loglevel, const char *filename, int line, int flags,
			const char *format, va_list ap);
extern void	mdebug(int loglevel, const char *filename, int line, int flags,
			const char *format, ...);

#endif /* _MDEBUG_H */
