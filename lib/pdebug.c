/*
 * pdebug.c -- use a redfined fprintf to redirect parser debugging messages
 *             into the common debug log system
 *
 * (c) 2002 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: pdebug.c,v 1.1 2002/01/27 21:01:43 afm Exp $
 */
#include <pdebug.h>
#include <stdarg.h>
#include <mdebug.h>

int	fprintf_debug(FILE *outfile, const char *format, ...) {
	va_list	ap;
	va_start(ap, format);
	vmdebug(LOG_DEBUG, __FILE__, __LINE__, MDEBUG_NOFILELINE, format, ap);
	va_end(ap);
	return 0;
}
