/*
 * printver.h -- simple function to print version information
 *
 * (c) 2002 Dr. Andreas Mueller, Beratung und Entwicklung 
 *
 * $Id: printver.h,v 1.3 2003/06/12 23:29:46 afm Exp $
 */
#ifndef _PRINTVER_H
#define _PRINTVER_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern void	fprintver(FILE *);

#ifdef __cplusplus
}
#endif

#endif /* _PRINTVER_H */
