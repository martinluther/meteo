/*
 * pdebug.h -- redefine the fprintf command so that the parser's debug
 *             messages can be sent to a debug location
 *
 * (c) 2002 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: pdebug.h,v 1.2 2003/06/06 15:11:05 afm Exp $
 */
#ifndef _PDEBUG_H
#define _PDEBUG_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define	fprintf	fprintf_debug
extern int	fprintf_debug(FILE *outfile, const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif /* _PDEBUG_H */
