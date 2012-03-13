/*
 * dograph.h -- create meteo data graphs similar to MRTGs
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: dograph.h,v 1.7 2003/06/12 23:29:46 afm Exp $
 */
#ifndef _DOGRAPH_H
#define _DOGRAPH_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <meteo.h>
#include <meteograph.h>
#include <graph.h>
#include <mysql.h>
#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <math.h>
#include <dewpoint.h>
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#include <errno.h>
#include <time.h>
#include <database.h>
#include <timestamp.h>

#ifdef __cplusplus
extern "C" {
#endif

#define	DOGRAPH_TEMPERATURE		1
#define	DOGRAPH_TEMPERATURE_INSIDE	1 << 1
#define	DOGRAPH_BAROMETER		1 << 2
#define	DOGRAPH_RAIN			1 << 3
#define	DOGRAPH_WIND			1 << 4
#define	DOGRAPH_RADIATION		1 << 5
#define	DOGRAPH_HUMIDITY		1 << 6
#define	DOGRAPH_HUMIDITY_INSIDE		1 << 7

typedef struct dograph_s {
	MYSQL		*mysql;
	const char	*prefix;
	const char	*suffix;
	const char	*timestamp;
	time_t		end;
	int		useaverages;
	int		requestedgraphs;
	int		withtimestamps;
	meteoconf_t	*mc;
} dograph_t;

extern graph_t 	*setup_graph(const dograph_t *dgp, char *query, int querylen,
			int interval, time_t *start, char *data, char *avgdata,
			char *nullclause);
extern int	set_colors(graph_t *graph, int channel, meteoconf_t *conf);
extern void	baro_graphs(dograph_t *dgp, int interval);
extern void	temp_graphs(dograph_t *dgp, int interval);
extern void	temp_graphs_inside(dograph_t *dgp, int interval);
extern void	humidity_graphs(dograph_t *dgp, int interval);
extern void	humidity_graphs_inside(dograph_t *dgp, int interval);
extern void	rain_graphs(dograph_t *dgp, int interval);
extern void	wind_graphs(dograph_t *dgp, int interval);
extern void	radiation_graphs(dograph_t *dgp, int interval);
extern void	all_graphs(dograph_t *dgp, int interval);

#ifdef __cplusplus
}
#endif

#endif /* _DOGRAPH_H */
