/*
 * dograph.c -- create meteo data graphs similar to MRTGs
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: dograph.c,v 1.10 2003/06/09 07:33:21 afm Exp $
 */
#include <meteo.h>
#include <meteograph.h>
#include <graph.h>
#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <dewpoint.h>
#include <string.h>
#include <time.h>
#include <timestamp.h>
#include <dograph.h>
#include <mdebug.h>

static gridscale_t	get_gridscale(meteoconf_t *mc, char *path, int interval,
				gridscale_t gridscaledefault) {
	gridscale_t	result;
	result.step = xmlconf_get_double(mc, path, "step", NULL,
		interval, gridscaledefault.step);
	result.start = xmlconf_get_double(mc, path, "start", NULL,
		interval, gridscaledefault.start);
	result.end = xmlconf_get_double(mc, path, "end", NULL,
		interval, gridscaledefault.end);
	return result;
}
#define	MINFTY	1.e68
static channelscale_t	get_channelscale(meteoconf_t *mc, char *path,
			int interval, channelscale_t channelscaledefault) {
	channelscale_t	result;
	double	max;
	max = xmlconf_get_double(mc, path, "max", NULL, interval, MINFTY);
	result.min = xmlconf_get_double(mc, path, "min", NULL,
		interval, channelscaledefault.min);
	result.scale = xmlconf_get_double(mc, path, "scale", NULL,
		interval, channelscaledefault.scale);
	if (max != MINFTY) {
		result.scale = (max - result.min)/(METEO_LLY - METEO_URY);
	}
	return result;
}

static void	create_filename(char *filename, int length,
		const dograph_t *dgp, char *parameter) {
	/* compute the file name differently with/without timestamps	*/
	if (dgp->withtimestamps) {
		snprintf(filename, length,
			"%s-%s-%s-%s.png",
			dgp->prefix, parameter, dgp->suffix, dgp->timestamp);
	} else {
		snprintf(filename, length,
			"%s-%s-%s.png",
			dgp->prefix, parameter, dgp->suffix);
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "filename: %s", filename);
}

#define	stampformat	"%Y%m%d%H%M%S"
graph_t 	*setup_graph(const dograph_t *dgp, char *query, int querylen,
	int interval, time_t *start, char *data, char *avgdata,
	char *nullclause) {
	graph_t	*g;
	time_t	end;

	/* set up the graph geometrically				*/
	g = graph_new(dgp->prefix, METEO_WIDTH, METEO_HEIGHT);
	graph_set_dimensions(g, METEO_LLX, METEO_LLY, METEO_URX, METEO_URY);

	/* set up the time margins for the graph			*/
	end = *start -= *start % interval;
	*start -= 400 * interval;

	/* formulate the query						*/
	if (dgp->useaverages)
		snprintf(query, querylen,
			"select timekey - %d, %s "
			"from averages "
			"where timekey between %d and %d "
			"  and station = '%-8.8s' "
			"  and intval = %d "
			"  and %s "
			"limit 400",
			interval, avgdata, (int)*start, (int)end,
			dgp->prefix, interval,
			(nullclause) ? nullclause : "0 = 0");
	else
		snprintf(query, querylen,
			"select group%d * %d, %s "
			"from stationdata "
			"where timekey between %d and %d "
			"  and station = '%-8.8s' "
			"  and %s "
			"group by group%d "
			"order by 1 "
			"limit 400",
			interval, interval, data, (int)*start, (int)end,
			dgp->prefix, (nullclause) ? nullclause : nullclause,
			interval);

	return g;
}

int	set_colors(graph_t *graph, int channel, meteoconf_t *conf) {
	ncolor_t	col;
	char	colorname[256];
	char	*thisname;

	/* first determine the name for the channel at hand		*/
	switch (channel) {
	case DOGRAPH_TEMPERATURE:
		thisname = "temperature";
		break;
	case DOGRAPH_TEMPERATURE_INSIDE:
		thisname = "temperature_inside";
		break;
	case DOGRAPH_HUMIDITY:
		thisname = "humidity";
		break;
	case DOGRAPH_HUMIDITY_INSIDE:
		thisname = "humidity_inside";
		break;
	case DOGRAPH_BAROMETER:
		thisname = "pressure";
		break;
	case DOGRAPH_RAIN:
		thisname = "rain";
		break;
	case DOGRAPH_WIND:
		thisname = "wind";
		break;
	case DOGRAPH_RADIATION:
		thisname = "radiation";
		break;
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "setting colors for '%s'", thisname);

	/* convert the name into a color name string and write set	*/
	/* the corresponding color					*/
	snprintf(colorname, sizeof(colorname), "channel[@name='%s']",
		thisname);
	col = xmlconf_get_color(conf, colorname, "color", "bgcolor", 0,
		defaultcolors.bgcolor);
	graph_set_color(graph, GRAPH_COLOR_BACKGROUND, col);

	col = xmlconf_get_color(conf, colorname, "color", "fgcolor", 0,
		defaultcolors.fgcolor);
	graph_set_color(graph, GRAPH_COLOR_FOREGROUND, col);

	col = xmlconf_get_color(conf, colorname, "color", "nodata", 0,
		defaultcolors.nodata);
	graph_set_color(graph, GRAPH_COLOR_NODATA, col);

	return 0;
}

/*
 * barometer graphs
 */
channelscale_t	pressure_default_channelscale = { 930., 0.5 };
gridscale_t	pressure_default_gridscale = { 10., 930., 980. };

void	baro_graphs(dograph_t *dgp, int interval) {
	graph_t		*g;
	char		query[2048], filename[1024];
	int		i, rangecolor, blue, nentries;
	entry_t		*data;
	double		*barodata;
	MYSQL_RES	*res;
	MYSQL_ROW	row;
	time_t		start;
	channelscale_t	cs;
	gridscale_t	gs;
	ticklabel_t	tl;

	start = dgp->end;
	g = setup_graph(dgp, query, sizeof(query), interval, &start,
		"max(barometer), min(barometer), avg(barometer)",
		"barometer_max, barometer_min, barometer",
		"barometer is not null");
	set_colors(g, DOGRAPH_BAROMETER, dgp->mc);
	graph_label(g, xmlconf_get_string(dgp->mc,
		"channel[@name='pressure']", "label", NULL,
		interval, "Pressure (hPa)"), 0);
	rangecolor = graph_color_allocate(g,
		xmlconf_get_color(dgp->mc,
			"channel[@name='pressure']", "color",
			"presure_range", interval,
			defaultcolors.pressure_range));
	blue = graph_color_allocate(g,
		xmlconf_get_color(dgp->mc,
			"channel[@name='pressure']", "color",
			"pressure", interval, defaultcolors.pressure));
	cs = get_channelscale(dgp->mc, "channel[@name='pressure']",
		interval, pressure_default_channelscale);
	graph_add_channel(g, GRAPH_HISTOGRAMM, rangecolor, cs);
	graph_add_channel(g, GRAPH_HISTOGRAMM | GRAPH_HIDE, g->bg, cs);
	graph_add_channel(g, GRAPH_LINE, blue, cs);

	/* retrieve the data from the database				*/
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "query is '%s'", query);
	if (mysql_query(dgp->mysql, query)) {
		mdebug(LOG_CRIT, MDEBUG_LOG, 0, "cannot retrieve data: %s",
			mysql_error(dgp->mysql));
		exit(EXIT_FAILURE);
	}
	res = mysql_store_result(dgp->mysql);

	/* find out how many rows there are				*/
	nentries = mysql_num_rows(res);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "baro query returns %d rows",
		nentries);

	/* allocate memory to store all the data			*/
	data = (entry_t *)malloc(sizeof(entry_t) * nentries);
	barodata = (double *)malloc(sizeof(double) * nentries * 3);

	/* read row by row and store in the data array			*/
	for (i = 0; i < nentries; i++) {
		row = mysql_fetch_row(res);
		data[i].when = atoi(row[0]);
		data[i].data = &barodata[i * 3];
		data[i].data[0] = atof(row[1]);
		data[i].data[1] = atof(row[2]);
		data[i].data[2] = atof(row[3]);
		if (debug > 1)
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
				"%9d %6.1f %6.1f %6.1f", (int)data[i].when,
				data[i].data[0], data[i].data[1],
				data[i].data[2]);
	}

	/* display the data retrieved from the database			*/
	graph_add_data(g, start, interval, nentries, data);

	/* display the grid						*/
	graph_add_time(g);
	gs = get_gridscale(dgp->mc, "channel[@name='pressure']",
		interval, pressure_default_gridscale);
	graph_add_grid(g, 0, gs);
	tl.gs = gs;
	tl.format = xmlconf_get_string(dgp->mc,
		"channel[@name='pressure']", "format", NULL,
		interval, "%.0f");
	graph_add_ticks(g, 0, tl, 0);

	/* compute the filename for the graph				*/
	create_filename(filename, sizeof(filename), dgp, "pressure");
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "pressure filename = %s", filename);
	graph_write_png(g, filename);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "image written to %s", filename);
	
	/* free the SQL data						*/
	mysql_free_result(res);
	free(barodata);
	free(data);
}

/*
 * humidity graphs
 */

#define	HUMIDITYSTRING	(inside) ? "humidity_inside" : "humidity"
#define	HUMIDITYSTRING1(a)	(inside) ? ("humidity_inside" a)	\
					 : ("humidity" a)

static channelscale_t	humidity_default_channelscale = { 0., 100. };
static gridscale_t	humidity_default_gridscale = { 20., 0., 100. };

static void	humidity_graphs_both(dograph_t *dgp, int interval, int inside) {
	graph_t		*g;
	char		query[2048], filename[1024];
	int		i, rangecolor, blue, nentries;
	entry_t		*data;
	double		*humiditydata;
	MYSQL_RES	*res;
	MYSQL_ROW	row;
	time_t		start;
	char		xpath[1024];
	channelscale_t	cs;
	gridscale_t	gs;
	ticklabel_t	tl;

	start = dgp->end;
	g = setup_graph(dgp, query, sizeof(query), interval, &start,
		(inside) ? "max(humidity_inside), min(humidity_inside), "
			   "avg(humidity_inside)"
		         : "max(humidity), min(humidity), avg(humidity)",
		(inside) ? "humidity_inside_max, humidity_inside_min, "
			   "humidity_inside"
			 : "humidity_max, humidity_min, humidity",
		(inside) ? "humidity_inside is not null"
			 : "humidity is not null");
	set_colors(g, (inside) ? DOGRAPH_HUMIDITY_INSIDE : DOGRAPH_HUMIDITY,
		dgp->mc);
	snprintf(xpath, sizeof(xpath), "channel[@name='%s']",
		HUMIDITYSTRING);
	graph_label(g,
		xmlconf_get_string(dgp->mc, xpath, "label", NULL, interval,
			"rel. Humidity (%)"), 0);
	rangecolor = graph_color_allocate(g,
		xmlconf_get_color(dgp->mc, xpath, "color",
			HUMIDITYSTRING1("_range"), interval,
			defaultcolors.humidity_range));
	blue = graph_color_allocate(g,
		xmlconf_get_color(dgp->mc, xpath, "color",
			HUMIDITYSTRING, interval,
			defaultcolors.humidity));

	cs = get_channelscale(dgp->mc, xpath,
		interval, humidity_default_channelscale);
	graph_add_channel(g, GRAPH_HISTOGRAMM, rangecolor, cs);
	graph_add_channel(g, GRAPH_HISTOGRAMM | GRAPH_HIDE, g->bg, cs);
	graph_add_channel(g, GRAPH_LINE, blue, cs);

	/* retrieve the data from the database				*/
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "query is '%s'", query);
	if (mysql_query(dgp->mysql, query)) {
		mdebug(LOG_CRIT, MDEBUG_LOG, 0, "cannot retrieve data: %s",
			mysql_error(dgp->mysql));
		exit(EXIT_FAILURE);
	}
	res = mysql_store_result(dgp->mysql);

	/* find out how many rows there are				*/
	nentries = mysql_num_rows(res);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
		"humidity query returns %d rows", nentries);

	/* allocate memory to store all the data			*/
	data = (entry_t *)malloc(sizeof(entry_t) * nentries);
	humiditydata = (double *)malloc(sizeof(double) * nentries * 3);

	/* read row by row and store in the data array			*/
	for (i = 0; i < nentries; i++) {
		row = mysql_fetch_row(res);
		data[i].when = atoi(row[0]);
		data[i].data = &humiditydata[i * 3];
		data[i].data[0] = atof(row[1]);
		data[i].data[1] = atof(row[2]);
		data[i].data[2] = atof(row[3]);
		if (debug > 1)
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
				"%9d %6.1f %6.1f %6.1f", (int)data[i].when,
				data[i].data[0], data[i].data[1],
				data[i].data[2]);
	}

	/* display the data retrieved from the database			*/
	graph_add_data(g, start, interval, nentries, data);

	/* display the grid						*/
	graph_add_time(g);
	gs = get_gridscale(dgp->mc, xpath, interval,
		humidity_default_gridscale);
	graph_add_grid(g, 0, gs);
	tl.gs = gs;
	tl.format = "%.0f";
	graph_add_ticks(g, 0, tl, 0);

	/* compute the filename for the graph				*/
	create_filename(filename, sizeof(filename), dgp, HUMIDITYSTRING);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "humidity filename = %s", filename);
	graph_write_png(g, filename);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "image written to %s", filename);
	
	/* free the SQL data						*/
	mysql_free_result(res);
	free(humiditydata);
	free(data);
}
void	humidity_graphs(dograph_t *dgp, int interval) {
	humidity_graphs_both(dgp, interval, 0);
}
void	humidity_graphs_inside(dograph_t *dgp, int interval) {
	humidity_graphs_both(dgp, interval, 1);
}

/*
 * temperature graphs
 */
#define	TEMPERATURESTRING	(inside) ? "temperature_inside" : "temperature"
#define	TEMPERATURESTRING1(a)	(inside) ? ("temperature_inside" a) \
					 : ("temperature" a)

static channelscale_t	temperature_default_channelscale = { -10., .5 };
static gridscale_t	temperature_default_gridscale = { 10., -10., 30. };

static void	temp_graphs_both(dograph_t *dgp, int interval, int inside) {
	time_t		start;
	graph_t		*g;
	char		query[2048], filename[1024];
	int		i, rangecolor, color, dewcolor, nentries;
	entry_t		*data;
	double		*tempdata;
	MYSQL_RES	*res;
	MYSQL_ROW	row;
	char		xpath[1024];
	channelscale_t	cs;
	gridscale_t	gs;
	ticklabel_t	tl;

	start = dgp->end;
	/* setup the temperature graph, depending on inside flag	*/
	if (inside) {
		g = setup_graph(dgp, query, sizeof(query), interval,
			&start,
			"ifnull(min(temperature_inside), -273.), "
			"ifnull(max(temperature_inside), -273.), "
			"ifnull(avg(temperature_inside), -273.), "
			"ifnull(avg(humidity_inside), 0.), ",
			"ifnull(temperature_inside_min, -273.), "
			"ifnull(temperature_inside_max, -273.), "
			"ifnull(temperature_inside, -273.), "
			"ifnull(humidity_inside, 0.)",
			"temperature_inside is not null");
		set_colors(g, DOGRAPH_TEMPERATURE_INSIDE, dgp->mc);
	} else {
		g = setup_graph(dgp, query, sizeof(query), interval,
			&start,
			"ifnull(min(temperature), -273.), "
			"ifnull(max(temperature), 100.), "
			"ifnull(avg(temperature), -273.), "
			"ifnull(avg(humidity), 0.)",
			"ifnull(temperature_min, -273.), "
			"ifnull(temperature_max, -273.), "
			"ifnull(temperature, -273.), "
			"ifnull(humidity, 0.)",
			"temperature is not null");
		set_colors(g, DOGRAPH_TEMPERATURE, dgp->mc);
	}
	
	/* adornments of the temperature graph				*/
	snprintf(xpath, sizeof(xpath), "channel[@name='%s']",
		TEMPERATURESTRING);
	graph_label(g, xmlconf_get_string(dgp->mc, xpath, "label", NULL,
			interval, "Temperature (deg C)"), 0);
	rangecolor = graph_color_allocate(g,
		xmlconf_get_color(dgp->mc, xpath, "color",
			TEMPERATURESTRING1("_range"),
			interval, defaultcolors.temperature_range));
	color = graph_color_allocate(g,
		xmlconf_get_color(dgp->mc, xpath, "color", 
			TEMPERATURESTRING,
			interval, defaultcolors.temperature));
	dewcolor = graph_color_allocate(g,
		xmlconf_get_color(dgp->mc, xpath, "color",
			TEMPERATURESTRING1("_dew"),
			interval, defaultcolors.temperature_dew));

	cs = get_channelscale(dgp->mc, xpath,
		interval, temperature_default_channelscale);
	graph_add_channel(g, GRAPH_HISTOGRAMM, rangecolor, cs);
	graph_add_channel(g, GRAPH_HISTOGRAMM | GRAPH_HIDE, g->bg, cs);
	graph_add_channel(g, GRAPH_LINE, dewcolor, cs);
	graph_add_channel(g, GRAPH_LINE, color, cs);

	/* retrieve the data from the database				*/
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "query is '%s'", query);
	if (mysql_query(dgp->mysql, query)) {
		mdebug(LOG_CRIT, MDEBUG_LOG, 0, "cannot retrieve data: %s",
			mysql_error(dgp->mysql));
		exit(EXIT_FAILURE);
	}
	res = mysql_store_result(dgp->mysql);

	/* find out how many rows there are				*/
	nentries = mysql_num_rows(res);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "temp query returns %d rows",
		nentries);

	/* allocate memory to store all the data			*/
	data = (entry_t *)malloc(sizeof(entry_t) * nentries);
	tempdata = (double *)malloc(sizeof(double) * nentries * 4);

	/* read row by row and store in the data array			*/
	for (i = 0; i < nentries; i++) {
		row = mysql_fetch_row(res);
		data[i].when = atoi(row[0]);
		data[i].data = &tempdata[i * 4];
		data[i].data[0] = atof(row[2]); /* max temperature      */
		data[i].data[1] = atof(row[1]); /* min temeprature      */
		data[i].data[3] = atof(row[3]); /* average temperature  */
		/* compute the dew point based on temperature/humidity  */
		data[i].data[2] = atof(row[4]);
		if (data[i].data[2] > 100.) data[i].data[2] = 100.;
		data[i].data[2] = dewpoint(data[i].data[2], data[i].data[3]);
		if (debug > 1)
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
				 "%9d %6.1f %6.1f %6.1f", (int)data[i].when,
				data[i].data[0], data[i].data[1],
				data[i].data[2]);
	}

	/* display the data retrieved from the database			*/
	graph_add_data(g, start, interval, nentries, data);

	/* display the grid						*/
	graph_add_time(g);
	gs = get_gridscale(dgp->mc, xpath, interval,
		temperature_default_gridscale);
	graph_add_grid(g, 0, gs);
	tl.gs = gs;
	tl.format = "%.0f";
	graph_add_ticks(g, 0, tl, 0);

	/* compute the filename for the graph				*/
	create_filename(filename, sizeof(filename), dgp, TEMPERATURESTRING);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "temperature filename = %s", filename);
	graph_write_png(g, filename);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "image written to %s", filename);
	
	/* free the SQL data						*/
	mysql_free_result(res);
	free(tempdata);
	free(data);
}
void	temp_graphs(dograph_t *dgp, int interval) {
	temp_graphs_both(dgp, interval, 0);
}
void	temp_graphs_inside(dograph_t *dgp, int interval) {
	temp_graphs_both(dgp, interval, 1);
}

/*
 * rain graphs
 */
static channelscale_t	rain_default_channelscale = { 0., 1. };
static gridscale_t	rain_default_gridscale = { 1., 0., 30. };

void	rain_graphs(dograph_t *dgp, int interval) {
	time_t		start;
	graph_t		*g;
	char		query[2048], filename[1024];
	int		i, color, nentries;
	entry_t		*data;
	double		*tempdata;
	MYSQL_RES	*res;
	MYSQL_ROW	row;
	channelscale_t	cs;
	gridscale_t	gs;
	ticklabel_t	tl;

	/* set up the rain query					*/
	start = dgp->end;
	g = setup_graph(dgp, query, sizeof(query), interval, &start,
		"sum(rain)", "rain", "rain is not null");
	set_colors(g, DOGRAPH_RAIN, dgp->mc);
	graph_label(g,
		xmlconf_get_string(dgp->mc, "channel[@name='rain']",
			"label", NULL, interval, "Precipitation (mm)"), 0);

	color = graph_color_allocate(g,
		xmlconf_get_color(dgp->mc, "channel[@name='rain']",
			"color", "rain", interval, defaultcolors.rain));
	
	cs = get_channelscale(dgp->mc, "channel[@name='rain']",
		interval, rain_default_channelscale);
	graph_add_channel(g, GRAPH_HISTOGRAMM, color,  cs);

	/* retrieve the data from the database				*/
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "query is '%s'", query);
	if (mysql_query(dgp->mysql, query)) {
		mdebug(LOG_CRIT, MDEBUG_LOG, 0, "cannot retrieve data: %s",
			mysql_error(dgp->mysql));
		exit(EXIT_FAILURE);
	}
	res = mysql_store_result(dgp->mysql);

	/* find out how many rows there are				*/
	nentries = mysql_num_rows(res);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "temp query returns %d rows",
		nentries);

	/* allocate memory to store all the data			*/
	data = (entry_t *)malloc(sizeof(entry_t) * nentries);
	tempdata = (double *)malloc(sizeof(double) * nentries);

	/* read row by row and store in the data array			*/
	for (i = 0; i < nentries; i++) {
		row = mysql_fetch_row(res);
		data[i].when = atoi(row[0]);
		data[i].data = &tempdata[i];
		data[i].data[0] = atof(row[1]);
		if (debug > 1)
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "%9d %6.1f",
				(int)data[i].when, data[i].data[0]);
	}

	/* display the data retrieved from the database			*/
	graph_add_data(g, start, interval, nentries, data);

	/* display the grid						*/
	graph_add_time(g);
	gs = get_gridscale(dgp->mc, "channel[@name='rain']", interval,
		rain_default_gridscale);
	graph_add_grid(g, 0, gs);
	tl.gs = gs;
	tl.format = "%.1f";
	graph_add_ticks(g, 0, tl, 0);

	/* compute the filename for the graph				*/
	create_filename(filename, sizeof(filename), dgp, "rain");
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "rain filename = %s", filename);
	graph_write_png(g, filename);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "image written to %s", filename);
	
	/* free the SQL data						*/
	mysql_free_result(res);
	free(tempdata);
	free(data);
}

/*
 * wind_background(graph_t *g)
 *
 * color the background for the wind direciton graphs if the color
 * attributes are configured.
 */
static void	wind_rectangle(graph_t *g, double miny, double maxy, int col) {
	int	mini, maxi;
	/* convert the max and min values to coordinate values		*/
	mini = graph_yval(g, 2, miny);
	maxi = graph_yval(g, 2, maxy);

	/* draw a rectangle						*/
	graph_rectangle(g, mini, maxi, col);
}

static void wind_background(meteoconf_t *meteoconf, graph_t *g, int interval) {
	ncolor_t	coln, cols, colw, cole;
	int	dc;
	/* check for north color					*/
	coln = xmlconf_get_color(meteoconf,
		"channel[@name='wind']/right", "color", "north",
		interval, defaultcolors.bgcolor);
	dc = gdImageColorAllocate(g->im, coln.c[0], coln.c[1], coln.c[2]);
	wind_rectangle(g, 0., 45., dc);
	wind_rectangle(g, 315., 360., dc);
	/* check for the west color					*/
	colw = xmlconf_get_color(meteoconf,
		"channel[@name='wind']/right", "color", "west",
		interval, defaultcolors.bgcolor);
	dc = gdImageColorAllocate(g->im, colw.c[0], colw.c[1], colw.c[2]);
	wind_rectangle(g, 225., 315., dc);
	/* check for the east color					*/
	cole = xmlconf_get_color(meteoconf,
		"channel[@name='wind']/right", "color", "east",
		interval, defaultcolors.bgcolor);
	dc = gdImageColorAllocate(g->im, cole.c[0], cole.c[1], cole.c[2]);
	wind_rectangle(g, 45., 135., dc);
	/* check for the south color					*/
	cols = xmlconf_get_color(meteoconf,
		"channel[@name='wind']/right", "color", "south",
		interval, defaultcolors.bgcolor);
	dc = gdImageColorAllocate(g->im, cols.c[0], cols.c[1], cols.c[2]);
	wind_rectangle(g, 135., 225., dc);
}

static channelscale_t	windspeed_default_channelscale = { 0., 0.2 };
static gridscale_t	windspeed_default_gridscale = { 5., 0., 15. };

static channelscale_t	winddir_default_channelscale = { -604., 8. };
static gridscale_t	winddir_default_gridscale = { 180., 0., 360. };

void	wind_graphs(dograph_t *dgp, int interval) {
	time_t		start;
	graph_t		*g;
	char		query[2048], filename[1024];
	int		i, speedcolor, gustcolor, dircolor, nentries;
	entry_t		*data;
	double		*winddata, x, y;
	MYSQL_RES	*res;
	MYSQL_ROW	row;
	channelscale_t	css, csd;
	gridscale_t	gss, gsd;
	ticklabel_t	tl;

	start = dgp->end;
	g = setup_graph(dgp, query, sizeof(query), interval, &start,
		"sum(windx), sum(windy), sum(duration), "
		"max(windgust)",
		"windgust, windspeed, winddir",
		"windspeed is not null");
	set_colors(g, DOGRAPH_WIND, dgp->mc);
	graph_label(g, xmlconf_get_string(dgp->mc,
		"channel[@name='wind']/left", "label", NULL,
		interval, "Speed (m/s)        "), 0);
	graph_label(g, xmlconf_get_string(dgp->mc,
		"channel[@name='wind']/right", "label", NULL,
		interval, "            Azimut (deg)"), 1);

	dircolor = graph_color_allocate(g, xmlconf_get_color(dgp->mc,
		"channel[@name='wind']", "color", "wind", interval,
		defaultcolors.wind));
	gustcolor = graph_color_allocate(g, xmlconf_get_color(dgp->mc,
		"channel[@name='wind']", "color", "gust", interval,
		defaultcolors.gust));
	speedcolor = graph_color_allocate(g, xmlconf_get_color(dgp->mc,
		"channel[@name='wind']", "color", "speed", interval,
		defaultcolors.speed));

	css = get_channelscale(dgp->mc, "channel[@name='wind']/left",
		interval, windspeed_default_channelscale);
	graph_add_channel(g, GRAPH_HISTOGRAMM, gustcolor, css);
	graph_add_channel(g, GRAPH_HISTOGRAMM, speedcolor, css);

	csd = get_channelscale(dgp->mc, "channel[@name='wind']/right",
		interval, winddir_default_channelscale);
	graph_add_channel(g, GRAPH_LINE, dircolor, csd);

	/* retrieve the data from the database				*/
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "query is '%s'", query);
	if (mysql_query(dgp->mysql, query)) {
		mdebug(LOG_CRIT, MDEBUG_LOG, 0, "cannot retrieve data: %s",
			mysql_error(dgp->mysql));
		exit(EXIT_FAILURE);
	}
	res = mysql_store_result(dgp->mysql);

	/* find out how many rows there are				*/
	nentries = mysql_num_rows(res);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "temp query returns %d rows");

	/* allocate memory to store all the data			*/
	data = (entry_t *)malloc(sizeof(entry_t) * nentries);
	winddata = (double *)malloc(sizeof(double) * nentries * 3);

	/* read row by row and store in the data array			*/
	for (i = 0; i < nentries; i++) {
		row = mysql_fetch_row(res);
		data[i].when = atoi(row[0]);
		data[i].data = &winddata[i * 3];
		if (dgp->useaverages) {
			data[i].data[0] = (row[1]) ? atof(row[1]) : 0.;
			data[i].data[1] = (row[2]) ? atof(row[2]) : 0.;
			data[i].data[2] = (row[3]) ? atof(row[3]) : 0.;
		} else {
			x = atof(row[1]); y = atof(row[2]);
			data[i].data[0] = atof(row[4]);
			data[i].data[1] = sqrt(x*x + y*y)/atof(row[3]);
			data[i].data[2] = 180. + 180. * atan2(x, y)/3.1415926535;
		}
		if (debug > 1)
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
				"%9d %6.1f %6.1f %6.0f", (int)data[i].when,
				data[i].data[0], data[i].data[1],
				data[i].data[2]);
	}

	/* display the wind direction background colors			*/
	wind_background(dgp->mc, g, interval);

	/* display the data retrieved from the database			*/
	graph_add_data(g, start, interval, nentries, data);

	/* display the grid						*/
	graph_add_time(g);
	gss = get_gridscale(dgp->mc, "channel[@name='wind']/left",
		interval, windspeed_default_gridscale);
	graph_add_grid(g, 0, gss);
	gsd = get_gridscale(dgp->mc, "channel[@name='wind']/right",
		interval, winddir_default_gridscale);
	graph_add_grid(g, 2, gsd);
	tl.gs = gss;
	tl.format = "%.0f";
	graph_add_ticks(g, 0, tl, 0);
	tl.gs = gsd;
	graph_add_ticks(g, 2, tl, 1);

	/* compute the filename for the graph				*/
	create_filename(filename, sizeof(filename), dgp, "wind");
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "wind filename = %s", filename);
	graph_write_png(g, filename);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "image written to %s", filename);
}

/*
 * radiation graphs
 */
channelscale_t	solar_default_channelscale = { 5, 0. };
gridscale_t	solar_default_gridscale = { 100., 0., 600.  };
channelscale_t	uv_default_channelscale = { 0.05, 0. };
gridscale_t	uv_default_gridscale = { 5, 0. };

void	radiation_graphs(dograph_t *dgp, int interval) {
	time_t		start;
	graph_t		*g;
	char		query[2048], filename[1024];
	int		i, solarcolor, uvcolor, nentries;
	entry_t		*data;
	double		*radiationdata;
	MYSQL_RES	*res;
	MYSQL_ROW	row;
	channelscale_t	css, csu;
	gridscale_t	gss, gsu;
	ticklabel_t	tl;

	start = dgp->end;
	g = setup_graph(dgp, query, sizeof(query), interval, &start,
		"avg(solar), avg(uv)",
		"solar, uv", "solar is not null and uv is not null");
	set_colors(g, DOGRAPH_RADIATION, dgp->mc);
	graph_label(g, xmlconf_get_string(dgp->mc,
		"channel[@name='radiation']/left", "label", NULL,
		interval, "Solar Radiation (W/m2)"), 0);
	graph_label(g, xmlconf_get_string(dgp->mc,
		"channel[@name='radiation']/right", "label", NULL,
		interval, "UV index"), 1);

	solarcolor = graph_color_allocate(g, xmlconf_get_color(dgp->mc,
		"channel[@name='radiation']/left", "color", "solar",
		interval, defaultcolors.solar));
	uvcolor = graph_color_allocate(g, xmlconf_get_color(dgp->mc,
		"channel[@name='radiation']/right", "color", "uv",
		interval, defaultcolors.uv));

	css = get_channelscale(dgp->mc,
		"channel[@name='radiation']/left", interval,
		solar_default_channelscale);
	graph_add_channel(g, GRAPH_HISTOGRAMM, solarcolor, css);

	csu = get_channelscale(dgp->mc,
		"channel[@name='radiation']/right", interval,
		uv_default_channelscale);
	graph_add_channel(g, GRAPH_LINE, uvcolor, csu);

	/* retrieve the data from the database				*/
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "query is '%s'", query);
	if (mysql_query(dgp->mysql, query)) {
		mdebug(LOG_CRIT, MDEBUG_LOG, 0, "cannot retrieve data: %s",
			mysql_error(dgp->mysql));
		exit(EXIT_FAILURE);
	}
	res = mysql_store_result(dgp->mysql);

	/* find out how many rows there are				*/
	nentries = mysql_num_rows(res);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "temp query returns %d rows",
		nentries);

	/* allocate memory to store all the data			*/
	data = (entry_t *)malloc(sizeof(entry_t) * nentries);
	radiationdata = (double *)malloc(sizeof(double) * nentries * 2);

	/* read row by row and store in the data array			*/
	for (i = 0; i < nentries; i++) {
		row = mysql_fetch_row(res);
		data[i].when = atoi(row[0]);
		data[i].data = &radiationdata[i * 2];
		data[i].data[0] = (row[1]) ? atof(row[1]) : 0.;
		data[i].data[1] = (row[2]) ? atof(row[2]) : 0.;
		if (debug > 1)
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "%9d %6.1f %6.1f",
				(int)data[i].when,
				data[i].data[0], data[i].data[1]);
	}

	/* display the data retrieved from the database			*/
	graph_add_data(g, start, interval, nentries, data);

	/* display the grid						*/
	graph_add_time(g);
	gss = get_gridscale(dgp->mc, 
		"channel[@name='radiation']/left", interval,
		solar_default_gridscale);
	graph_add_grid(g, 0, gss);

	gsu = get_gridscale(dgp->mc, 
		"channel[@name='radiation']/right", interval,
		uv_default_gridscale);
	/* it's not necessary to write the grid lines twice...
	graph_add_grid(g, 1, gsu);
	*/
	
	tl.gs = gss;
	tl.format = "%.0f";
	graph_add_ticks(g, 0, tl, 0);
	tl.gs = gsu;
	graph_add_ticks(g, 1, tl, 1);

	/* compute the filename for the graph				*/
	create_filename(filename, sizeof(filename), dgp, "radiation");
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "radiation filename = %s", filename);
	graph_write_png(g, filename);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "image written to %s", filename);
}

/*
 * all graphs
 */
void	all_graphs(dograph_t *dgp, int interval) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "creating all graphs for "
		"interval %d, ending at %d, suffix = '%s'",
		interval, (int)dgp->end, dgp->suffix);
	if (dgp->requestedgraphs & DOGRAPH_BAROMETER)
		baro_graphs(dgp, interval);
	if (dgp->requestedgraphs & DOGRAPH_TEMPERATURE)
		temp_graphs(dgp, interval);
	if (dgp->requestedgraphs & DOGRAPH_TEMPERATURE_INSIDE)
		temp_graphs_inside(dgp, interval);
	if (dgp->requestedgraphs & DOGRAPH_HUMIDITY)
		humidity_graphs(dgp, interval);
	if (dgp->requestedgraphs & DOGRAPH_HUMIDITY_INSIDE)
		humidity_graphs_inside(dgp, interval);
	if (dgp->requestedgraphs & DOGRAPH_RAIN)
		rain_graphs(dgp, interval);
	if (dgp->requestedgraphs & DOGRAPH_WIND)
		wind_graphs(dgp, interval);
	if (dgp->requestedgraphs & DOGRAPH_RADIATION)
		radiation_graphs(dgp, interval);
}

