/*
**  meteo_vers.c -- Version Information for Meteo (syntax: C/C++)
**  [automatically generated and maintained by GNU shtool]
*/

#ifdef _METEO_VERS_C_AS_HEADER_

#ifndef _METEO_VERS_C_
#define _METEO_VERS_C_

#define METEO_VERSION 0x003200

typedef struct {
    const int   v_hex;
    const char *v_short;
    const char *v_long;
    const char *v_tex;
    const char *v_gnu;
    const char *v_web;
    const char *v_sccs;
    const char *v_rcs;
} meteo_version_t;

extern meteo_version_t meteo_version;

#endif /* _METEO_VERS_C_ */

#else /* _METEO_VERS_C_AS_HEADER_ */

#define _METEO_VERS_C_AS_HEADER_
#include "meteo_vers.c"
#undef  _METEO_VERS_C_AS_HEADER_

meteo_version_t meteo_version = {
    0x003200,
    "0.3.0",
    "0.3.0 (19-Jan-2002)",
    "This is Meteo, Version 0.3.0 (19-Jan-2002)",
    "Meteo 0.3.0 (19-Jan-2002)",
    "Meteo/0.3.0",
    "@(#)Meteo 0.3.0 (19-Jan-2002)",
    "$Id: shtool,v 1.1 2001/03/18 15:07:34 afm Exp Meteo 0.3.0 (19-Jan-2002) $"
};

#endif /* _METEO_VERS_C_AS_HEADER_ */

