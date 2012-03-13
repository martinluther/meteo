/*
 * dograph.h -- create meteo data graphs similar to MRTGs
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: dograph.h,v 1.2 2002/01/11 19:35:19 afm Exp $
 */
#ifndef _DOGRAPH_H
#define _DOGRAPH_H

#include <meteo.h>
#include <meteograph.h>
#include <mconf.h>
#include <graph.h>
#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <dewpoint.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <database.h>
#include <timestamp.h>

#define	DOGRAPH_TEMPERATURE		1
#define	DOGRAPH_TEMPERATURE_INSIDE	1 << 1
#define	DOGRAPH_BAROMETER		1 << 2
#define	DOGRAPH_RAIN			1 << 3
#define	DOGRAPH_WIND			1 << 4
#define	DOGRAPH_RADIATION		1 << 5

typedef struct dograph_s {
	MYSQL		*mysql;
	const char	*prefix;
	const char	*suffix;
	time_t		end;
	int		useaverages;
	int		requestedgraphs;
} dograph_t;

extern graph_t 	*setup_graph(const dograph_t *dgp, char *query, int querylen,
			int interval, time_t *start, char *data, char *avgdata);
extern int	set_colors(graph_t *graph, int channel, mc_node_t *conf);
extern void	baro_graphs(dograph_t *dgp, int interval);
extern void	temp_graphs(dograph_t *dgp, int interval);
extern void	temp_graphs_inside(dograph_t *dgp, int interval);
extern void	rain_graphs(dograph_t *dgp, int interval);
extern void	wind_graphs(dograph_t *dgp, int interval);
extern void	radiation_graphs(dograph_t *dgp, int interval);
extern void	all_graphs(dograph_t *dgp, int interval);

#endif /* _DOGRAPH_H */
