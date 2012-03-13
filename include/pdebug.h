/*
 * pdebug.h -- redefine the fprintf command so that the parser's debug
 *             messages can be sent to a debug location
 *
 * (c) 2002 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: pdebug.h,v 1.1 2002/01/27 21:01:42 afm Exp $
 */
#ifndef _PDEBUG_H
#define _PDEBUG_H

#include <stdio.h>

#define	fprintf	fprintf_debug
extern int	fprintf_debug(FILE *outfile, const char *format, ...);

#endif /* _PDEBUG_H */
