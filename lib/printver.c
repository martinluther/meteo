/*
 * printver.c -- print version information
 *
 * (c) 2002  Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: printver.c,v 1.1 2002/01/18 23:34:30 afm Exp $
 */
#include <printver.h>


#define _METEO_VERS_C_AS_HEADER_
#include "meteo_vers.c"

void	fprintver(FILE *o) {
	fprintf(o, "%s\n", meteo_version.v_gnu);
	fprintf(o, "default configuration file: %s\n", METEOCONFFILE);
}
