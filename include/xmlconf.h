/*
 * xmlconf.h -- declarations of functions used to read an XML configuration
 *              file and provide access to its contents
 *
 * (c) 2002 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: xmlconf.h,v 1.2 2003/06/06 15:11:05 afm Exp $
 */
#ifndef _XMLCONF_H
#define _XMLCONF_H

#include <libxml/tree.h>
#include <colors.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct meteoconf_s {
	xmlDocPtr	xml;
	char		*station;
} meteoconf_t;

/* read the configuration file						*/
extern xmlDocPtr	xmlconf_open(const char *filename);
extern void		xmlconf_close(xmlDocPtr xmlconf);

extern meteoconf_t	*xmlconf_new(const char *filename, const char *station);
extern void		xmlconf_free(meteoconf_t *);

/* get absolute string values						*/
extern const char	*xmlconf_get_abs_string(const meteoconf_t *mc,
	const char *path, const char *defaultvalue);
extern const ncolor_t	xmlconf_get_abs_color(const meteoconf_t *mc,
	const char *path, ncolor_t defaultvalue);
extern double	xmlconf_get_abs_double(const meteoconf_t *mc,
	const char *path, double defaultvalue);
extern int	xmlconf_get_abs_int(const meteoconf_t *mc,
	const char *path, int defaultvalue);

/* access to data for channels						*/
extern double	xmlconf_get_double(const meteoconf_t *mc,
	const char *initialpath, const char *element, const char *name,
	int interval, double defaultvalue);
extern int	xmlconf_get_int(const meteoconf_t *mc,
	const char *initialpath, const char *element, const char *name,
	int interval, int defaultvalue);
extern const char	*xmlconf_get_string(const meteoconf_t *mc,
	const char *initialpath, const char *element, const char *name,
	int interval, const char *defaultvalue);
extern ncolor_t	xmlconf_get_color(const meteoconf_t *mc,
	const char *initialpath, const char *element, const char *name,
	int interval, ncolor_t defaultvalue);

#ifdef __cplusplus
}
#endif

#endif /* _XMLCONF_H */
