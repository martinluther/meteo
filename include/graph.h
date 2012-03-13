/*
 * graph.h -- given a table of 400 entries, create a graph as mrtg does
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: graph.h,v 1.4 2003/06/06 15:11:05 afm Exp $
 */
#ifndef _GRAPH_H
#define _GRAPH_H

#include <gd.h>
#include <colors.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct entry	{
	time_t	when;
	double	*data;
} entry_t;

typedef struct channelformat {
	int	flags;
	int	color;
	double	offset;
	double	scale;
	double	max;
} channelformat_t;

#define	GRAPH_COLOR_FOREGROUND	0
#define	GRAPH_COLOR_BACKGROUND	1
#define	GRAPH_COLOR_NODATA	2

typedef struct graph {
	char		*prefix;
	int		width, height;		/* image dimensions	*/
	int		llx, lly, urx, ury;	/* graph part dimensions*/
	time_t		start;
	int		interval;
	gdImagePtr	im;
	int		nchannels;
	channelformat_t	*channelfmt;
	int		nentries;
	entry_t		*data;
	int		bg, fg, nodatacolor;
} graph_t;

typedef struct channelscale_s {
	double	min;
	double	scale;
} channelscale_t;

typedef	struct gridscale_s {
	double	step, start, end;
} gridscale_t;

typedef struct ticklabel_s {
	gridscale_t	gs;
	const char	*format;
} ticklabel_t;

#define	GRAPH_HISTOGRAMM	1
#define	GRAPH_LINE		2
#define	GRAPH_HIDE		(1 << 8)

extern graph_t	*graph_new(const char *prefix, int width, int height);
extern void	graph_free(graph_t *graph);
extern void	graph_set_dimensions(graph_t *, int llx, int lly, int urx,
		int ury);
extern int	graph_set_color(graph_t *, int whichcolor, const ncolor_t rgb);
extern int	graph_color_allocate(graph_t *, const ncolor_t rgb);
extern void	graph_add_channel(graph_t *g, int flags, int color,
		channelscale_t cs);
extern void	graph_add_data(graph_t *, time_t start, int interval,
		int nentries, entry_t *data);
extern void	graph_add_grid(graph_t *, int channel, gridscale_t gs);
extern void	graph_add_ticks(graph_t *, int channel, ticklabel_t ts,
		int onright);
extern int	graph_write_png(graph_t *, char *);
extern void	graph_label(graph_t *, const char *label, int onright);
extern void	graph_add_time(graph_t *);
extern void	graph_rectangle(graph_t *, int miny, int maxy, int col);
extern int	graph_yval(graph_t *, int channel, double y);

#ifdef __cplusplus
}
#endif

#endif /* _GRAPH_H */
