/*
 * mdebug.h -- meteo debugging subsystem
 *
 * (c) 2002 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: mdebug.h,v 1.3 2003/06/12 23:29:46 afm Exp $
 */
#ifndef _MDEBUG_H
#define _MDEBUG_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#ifdef HAVE_SYSLOG_H
#include <syslog.h>
#endif
#include <stdarg.h>
#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif /* _MDEBUG_H */
