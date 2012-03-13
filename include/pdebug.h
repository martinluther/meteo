/*
 * pdebug.h -- redefine the fprintf command so that the parser's debug
 *             messages can be sent to a debug location
 *
 * (c) 2002 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: pdebug.h,v 1.3 2003/06/12 23:29:46 afm Exp $
 */
#ifndef _PDEBUG_H
#define _PDEBUG_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define	fprintf	fprintf_debug
extern int	fprintf_debug(FILE *outfile, const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif /* _PDEBUG_H */
