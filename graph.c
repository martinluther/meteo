/*
 * graph.c -- given a table of 400 entries, create a graph as mrtg does
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: graph.c,v 1.8 2001/12/26 22:10:45 afm Exp $
 */
#include <stdlib.h>
#include <stdio.h>
#include <graph.h>
#include <gdfonts.h>
#include <meteo.h>
#include <time.h>

/*
 * graph_new	create a new graph structure
 */
graph_t	*graph_new(const char *prefix, int width, int height) {
	graph_t	*g;
	g = (graph_t *)malloc(sizeof(graph_t));
	if (debug)
		fprintf(stderr, "%s:%d: (graph_t *)malloc(%d) = %p, "
			"prefix = %s\n",
			__FILE__, __LINE__, sizeof(graph_t), g,
			(NULL != prefix) ? prefix : "(null)");
	memset(g, 0, sizeof(graph_t));
	g->prefix = strdup(prefix);
	g->width = width;
	g->height = height;
	g->im = gdImageCreate(width, height);
	g->bg = gdImageColorAllocate(g->im, 255, 255, 255);
	g->fg = gdImageColorAllocate(g->im, 0, 0, 0);
	g->red = gdImageColorAllocate(g->im, 255, 0, 0);
	return g;
}

/*
 * graph_free	destroy the graph structure
 */
void	graph_free(graph_t *g) {
	int	i;

	/* destroy the prefix string					*/
	if (g->prefix) {
		free(g->prefix); g->prefix = NULL;
	}

	/* destroy the embedded image					*/
	gdImageDestroy(g->im); g->im = NULL;

	/* destroy the embedded channel format array			*/
	if (g->channelfmt != NULL) {
		free(g->channelfmt);
		g->channelfmt = NULL;
		g->nchannels = 0;
	}

	/* destroy the embedded data array				*/
	for (i = 0; i < g->nentries; i++) {
		if (g->data[i].data)
			free(g->data[i].data);
	}
	if (g->data) {
		free(g->data);
		g->data = NULL;
		g->nentries = 0;
	}

	/* free the data structure					*/
	if (debug)
		fprintf(stderr, "%s:%d: free((graph_t *)%p)\n", __FILE__,
			__LINE__, g);
	free(g);
}

/*
 * graph_set_dimensions	set the horizontal and vertical dimensions of the
 *			graph inside the image, the size of which was already
 *			specified in the graph_new call
 */
void	graph_set_dimensions(graph_t *g, int llx, int lly, int urx, int ury) {
	g->llx = llx;
	g->lly = lly;
	g->urx = urx;
	g->ury = ury;
}

#define	tm(g,t)	(g->start + t * g->interval)
#define	round(d)	((int)(d + 0.5))
static int	ycoord(graph_t *g, channelformat_t *cf, double y) {
	return g->ury + round((cf->max - y)/cf->scale);
}
static double	maxvalue(graph_t *g, channelformat_t *cf) {
	return cf->offset - cf->scale * (g->ury - g->lly);
}


/*
 * graph_color_allocate	allocate a new color in the gdb image
 */
int	graph_color_allocate(graph_t *g, const int *rgb) {
	return gdImageColorAllocate(g->im, rgb[0], rgb[1], rgb[2]);
}

/*
 * graph_add_channel	add a channel specification to the graph
 */
void	graph_add_channel(graph_t *g, int flags, int color,
	double offset, double scale) {
	channelformat_t	*gf;
	g->nchannels++;
	if (debug)
		fprintf(stderr, "%s:%d: adding channel: now %d\n",
			__FILE__, __LINE__, g->nchannels);
	g->channelfmt = (channelformat_t *)realloc(g->channelfmt,
		g->nchannels * sizeof(channelformat_t));
	gf = &g->channelfmt[g->nchannels - 1];
	gf->flags = flags;
	gf->color = color;
	gf->offset = offset;
	gf->scale = scale;
	gf->max = maxvalue(g, gf);
}

/*
 * graph_channel	compute the graph curve and add it to the image, the
 *			data must have been added previously with a call
 *			to graph_add_data
 */
static void	graph_channel(graph_t *g, int channel) {
	int		flags, color, twidth, previousx = -1, previousy = -1,
			tmpx, tmpy, t, i;
	channelformat_t	*cf;
	int		x1, y1, x2, y2;
	time_t		when;
	double		offset, scale, max, value;
#define	px(x)	(tmpx = (previousx < 0) ? x : previousx, 	\
		previousx = x, tmpx)
#define	py(y)	(tmpy = (previousy < 0) ? y : previousy, 		\
		previousy = y, tmpy)

	/* get graph parameters						*/
	cf = &g->channelfmt[channel];
	flags = cf->flags;
	color = cf->color;
	offset = cf->offset;
	scale = cf->scale;
	max = cf->max;
	if (debug)
		fprintf(stderr, "%s:%d: graphing channel %d, flags = %0x, "
			" color = %d, offset = %f, scale = %f\n", __FILE__,
			__LINE__, channel, flags, color, offset, scale);

	twidth = g->urx - g->llx;
	if (debug)
		fprintf(stderr, "%s:%d: max channel value: %f, entries: %d\n",
			__FILE__, __LINE__, max, g->nentries);
	if (debug)
		fprintf(stderr, "%s:%d: parameters: start %d, "
			"interval: %d\n", __FILE__, __LINE__, (int)g->start,
			g->interval);

	/*
	 * the for loop below uses the following logic:
	 * it processes the data points in the array g->data, which contains
	 * g->nentries data points. t is the offset along the x-axis of the
	 * graph, we step ahead one pixel at a time to find the value that
	 * corresponds to the time value of data point, this is done in
	 * the inner for loop. The macro tm(g, t) returns the time at pixel
	 * t based on the graph g.
	 */
	t = -1;
	for (i = 0; i < g->nentries; i++) {
		/* find the time for the next data point we have	*/
		when = g->data[i].when;
		if (debug > 1)
			fprintf(stderr, "%s:%d: data[%d].when = %d\n",
				__FILE__, __LINE__, i, (int)when);

		/* skip to the next data point				*/
		for (t = t + 1; ((t < twidth) && (tm(g,t) < when)) ; t++);
		if (t >= twidth) {
			if (debug > 1)
				fprintf(stderr, "%s:%d: time exceeded on "
				"the right\n", __FILE__, __LINE__);
			break;
		}
		if (debug > 1)
			fprintf(stderr, "%s:%d: key %ld at x = %d\n", __FILE__,
				__LINE__, tm(g,t), t);

		/* find the value of the data point and plot it		*/
		value = g->data[i].data[channel];
		if (debug > 1)
			fprintf(stderr, "%s:%d: point (%ld, %f)\n", __FILE__,
				__LINE__, when, value);
		if ((value <= max) && (value >= offset)) {
			if (flags & GRAPH_LINE) {
				x2 = g->llx + t;
				y2 = ycoord(g, cf, value);
				x1 = px(x2);
				y1 = py(y2);
			}
			if (flags & GRAPH_HISTOGRAMM) {
				x1 = g->llx + t;
				y1 = g->lly;
				x2 = x1;
				y2 = ycoord(g, cf, value);
				if (flags & GRAPH_HIDE) y2--;
			}
			if (debug > 1)
				fprintf(stderr, "%s:%d: drawing line "
					"(%d,%d)-(%d,%d)\n", __FILE__,
					__LINE__, x1, y1, x2, y2);
			gdImageLine(g->im, x1, y1, x2, y2, color);
		} else {
			if (debug)
				fprintf(stderr, "%s:%d: point (%ld, %f) out "
					"of range\n", __FILE__, __LINE__,
					when, value);
		}
	}
}

/*
 * graph_add_data	add the data of a given channel to the graph, the
 *			array data most contain all channels
 */
void	graph_add_data(graph_t *g, time_t start, int interval, int nentries,
	entry_t *data) {
	int	i;
	g->start = start;
	g->interval = interval;
	g->nentries = nentries;
	g->data = data;
	for (i = 0; i < g->nchannels; i++) {
		graph_channel(g, i);
	}
}


/*
 * graph_add_time	add the time ticks and vertical grid lines to the graph
 *
 * as the graph already knows all the information to compute the time ticks
 * no other parameters as the graph are required
 */
void	graph_add_time(graph_t *g) {
	int		skip, t, x, y;
	int		styleDotted[3];
	time_t		tt;
	struct tm	*lt;
	char		label[128];

	/* define the dotted style for ``internal'' grid lines		*/
	styleDotted[0] = gdTransparent;
	styleDotted[1] = g->fg;
	styleDotted[2] = gdTransparent;
	gdImageSetStyle(g->im, styleDotted, 3);

	/* draw vertical grid lines and tick marks on time axis		*/
	for (t = -42; t < 400; t++) {
		/* find out whether this has minute/hour/week/month	*/
		/* equal to zero					*/
		tt = g->start + g->interval * t;
		lt = localtime(&tt);
		switch (g->interval) {
		case 300:
			/* draw a line every hour			*/
			if (lt->tm_min != 0)
				goto noline;
			/* label the line every second hour		*/
			if ((lt->tm_hour % 2) != 0)
				goto nolabel;
			strftime(label, sizeof(label), "%H", lt);
			skip = 0;	/* place label below line	*/
			break;
		case 1800:
			/* draw a tick every day			*/
			if ((lt->tm_hour != 0) || (lt->tm_min != 0))
				goto noline;
			strftime(label, sizeof(label), "%a", lt);
			skip = 24;	/* 24 lines = 1/2 day		*/
			break;
		case 7200:
			/* draw a tick every week			*/
			if ((lt->tm_min != 0) || (lt->tm_wday != 0))
				goto noline;
			/* only one of the following two conditions 	*/
			/* will be true, so there is no danger for	*/
			/* duplicate lines				*/
			if ((lt->tm_hour != 0) && (lt->tm_hour != 1))
				goto noline;
			strftime(label, sizeof(label), "Week %W", lt);
			skip = 42;	/* 42 lines = 84h = 1/2 week	*/
			break;
		case 86400:
			/* draw a tick every month			*/
			if (lt->tm_mday != 1)
				goto noline;
			strftime(label, sizeof(label), "%b", lt);
			skip = 15;	/* 15 lines = 15 d = 1/2 month	*/
			break;
		}
		/* draw the label					*/
		x = g->llx + t + skip;
		if ((x >= g->llx) && (x <= g->urx)) {
			x -= strlen(label) * gdFontSmall->w * 0.5;
			y = g->lly + 3;
			gdImageString(g->im, gdFontSmall, x, y,
				(unsigned char *)label, g->fg);
		}
	nolabel:
		/* draw the line, label has already been written	*/
		x = g->llx + t;
		if ((x >= g->llx) && (x <= g->urx)) {
			gdImageLine(g->im, x, g->lly, x, g->ury, gdStyled);
			gdImageLine(g->im, x, g->lly - 2, x, g->lly + 2, g->fg);
		}
	noline:
		;
	}

}

/*
 * graph_add_grid	add the horizontal grid lines to the graph
 *
 * parameters:
 *   channel	the channel from which to take the scale information
 *   valint	the step interval between lines
 *   from	the value of the lowest tick
 *   to		the value of the largest tick
 */
void	graph_add_grid(graph_t *g, int channel, double valint, double from,
	double to) {
	int		width, i, y, max;
	int		styleDotted[3];
	channelformat_t	*cf;
	double		vy;

	cf = &g->channelfmt[channel];

	max = maxvalue(g, cf);
	width = g->urx - g->llx;
	if (debug)
		fprintf(stderr, "%s:%d: adding grid starting at %ld, "
			"interval %d, based on channel %d, "
			"valint = %f\n", __FILE__, __LINE__,
			g->start, g->interval, channel, valint);

	/* define the dotted style for ``internal'' grid lines		*/
	styleDotted[0] = gdTransparent;
	styleDotted[1] = g->fg;
	styleDotted[2] = gdTransparent;
	gdImageSetStyle(g->im, styleDotted, 3);

	/* draw horizontal grid lines and tick marks on both data axes,	*/
	/* as well as labels for the 					*/
	if (debug)
		fprintf(stderr, "%s:%d: drawing vertical lines for "
			"offset = %f, interval = %f\n", __FILE__, __LINE__,
			cf->offset, valint);
	for (i = (from/valint);
		vy = i * valint, y = ycoord(g, cf, vy), y >= g->ury; i++) {
		if (debug)
			fprintf(stderr, "%s:%d: try line for i = %d, y = %f, "
				" y coord = %d\n", __FILE__, __LINE__,
				i, vy, y);
		if ((vy < from) || (vy > to))
			goto noline2;
		gdImageLine(g->im, g->llx, y, g->urx, y, gdStyled);
	noline2:
		; /* keep compiler happy */
	}

	/* draw border							*/
	gdImageLine(g->im, g->llx, g->lly, g->llx, g->ury, g->fg);	/* | */
	gdImageLine(g->im, g->urx, g->lly, g->urx, g->ury, g->fg);	/* | */
	gdImageLine(g->im, g->llx, g->lly, g->urx, g->lly, g->fg);	/* _ */
	gdImageLine(g->im, g->llx, g->ury, g->urx, g->ury, g->fg);	/* _ */
}

/*
 * graph_add_ticks	add the horizontal ticks and numbers to the graph
 *
 * parameters:
 *   channel	the channel from which to take the scale information
 *   valint	the step interval between lines
 *   from	the value of the lowest tick
 *   to		the value of the largest tick
 *   onright	whether or not to put the ticks on the right (0 = left,
 *		1 = right)
 */
void	graph_add_ticks(graph_t *g, int channel, double valint, double from,
	double to, const char *fmt, int onright) {

	int		width, i, y, max, xl, yl;
	channelformat_t	*cf;
	char		label[128];
	double		vy;

	cf = &g->channelfmt[channel];

	max = maxvalue(g, cf);
	width = g->urx - g->llx;
	if (debug)
		fprintf(stderr, "%s:%d: adding ticks starting at %ld, "
			"interval %d, based on channel %d, "
			"valint = %f\n", __FILE__, __LINE__,
			g->start, g->interval, channel, valint);

	/* draw horizontal grid lines and tick marks on both data axes,	*/
	/* as well as labels for the 					*/
	if (debug)
		fprintf(stderr, "%s:%d: drawing vertical lines for "
			"offset = %f, interval = %f\n", __FILE__, __LINE__,
			cf->offset, valint);
	for (i = from/valint;
		vy = i * valint, y = ycoord(g, cf, vy), y >= g->ury; i++) {
		if (debug)
			fprintf(stderr, "%s:%d: try tick for i = %d, y = %f, "
				" y coord = %d\n", __FILE__, __LINE__,
				i, vy, y);
		if ((vy < from) || (vy > to))
			goto noline2;
		snprintf(label, 128, fmt, vy);
		if (onright) {
			gdImageLine(g->im, g->urx - 2, y, g->urx + 2, y, g->fg);
			xl = g->urx + 2 + 0.5 * gdFontSmall->w;
		} else {
			gdImageLine(g->im, g->llx - 2, y, g->llx + 2, y, g->fg);
			xl = g->llx - 2 - (strlen(label) + 0.5)
				* gdFontSmall->w;
		}
		yl = y - gdFontSmall->h/2;
		gdImageString(g->im, gdFontSmall, xl, yl,
			(unsigned char *)label, g->fg);
	noline2:
		; /* keep comiler happy */
	}
}

/*
 * graph_write_png	write a graph as PNG file
 */
int	graph_write_png(graph_t *g, char *filename) {
	FILE	*file;
	if (NULL == (file = fopen(filename, "wb"))) {
		fprintf(stderr, "%s:%d: cannot write to file %s\n", __FILE__,
			__LINE__, filename);
		return -1;
	}
	gdImagePng(g->im, file);
	fclose(file);
	return 0;
}

/*
 * graph_label	add a label to the graph
 */
void	graph_label(graph_t *g, const char *label, int onright) {
	int	x;
	if (onright)
		/* x coordinate for label on right hand side of graph	*/
		x = g->width - gdFontSmall->h - 5;
	else
		/* x coordinate for label on left hand side of grah	*/
		x = -8 + gdFontSmall->h;
	gdImageStringUp(g->im, gdFontSmall, x,
		(strlen(label) * gdFontSmall->w)/2 + (g->height / 2),
		(unsigned char *)label, g->fg);
}
