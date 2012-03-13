/*
 * dograph.c -- create meteo data graphs similar to MRTGs
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: dograph.c,v 1.6 2002/08/24 14:56:21 afm Exp $
 */
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
#include <time.h>
#include <timestamp.h>
#include <dograph.h>
#include <mdebug.h>

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
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "filename: %s", filename);
}

#define	stampformat	"%Y%m%d%H%M%S"
graph_t 	*setup_graph(const dograph_t *dgp, char *query, int querylen,
	int interval, time_t *start, char *data, char *avgdata) {
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
			"limit 400",
			interval, avgdata, (int)*start, (int)end,
			dgp->prefix, interval);
	else
		snprintf(query, querylen,
			"select group%d * %d, %s "
			"from stationdata "
			"where timekey between %d and %d "
			"  and station = '%-8.8s' "
			"group by group%d "
			"order by 1 "
			"limit 400",
			interval, interval, data, (int)*start, (int)end,
			dgp->prefix, interval);

	return g;
}

static color_t	default_fgcolor = { 0, 0, 0 };
static color_t	default_bgcolor = { 255, 255, 255 };

int	set_colors(graph_t *graph, int channel, mc_node_t *conf) {
	const int	*col;
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
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "setting colors for '%s'",
			thisname);

	/* convert the name into a color name string and write set	*/
	/* the corresponding color					*/
	snprintf(colorname, sizeof(colorname), "%s.bgcolor", thisname);
	col = mc_get_color(conf, colorname, default_bgcolor);
	graph_set_color(graph, GRAPH_COLOR_BACKGROUND, col);

	snprintf(colorname, sizeof(colorname), "%s.fgcolor", thisname);
	col = mc_get_color(conf, colorname, default_fgcolor);
	graph_set_color(graph, GRAPH_COLOR_FOREGROUND, col);

	snprintf(colorname, sizeof(colorname), "%s.nodatacolor", thisname);
	col = mc_get_color(conf, colorname, NULL);
	if (NULL != col)
		graph_set_color(graph, GRAPH_COLOR_NODATA, col);

	return 0;
}

/*
 * barometer graphs
 */
static color_t	pressure_default_color = { 127, 127, 255 };
static color_t	pressure_default_rangecolor = { 210, 210, 255 };
#define	PRESSURESCALEDEFAULT	0.5
#define	PRESSUREMINDEFAULT	930.
#define	PRESSUREMAXDEFAULT	980.
#define	PRESSURESTEPDEFAULT	10.

void	baro_graphs(dograph_t *dgp, int interval) {
	graph_t		*g;
	char		query[2048], filename[1024];
	int		i, rangecolor, blue, nentries;
	entry_t		*data;
	double		*barodata;
	MYSQL_RES	*res;
	MYSQL_ROW	row;
	time_t		start;

	start = dgp->end;
	g = setup_graph(dgp, query, sizeof(query), interval, &start,
		"max(barometer), min(barometer), avg(barometer)",
		"barometer_max, barometer_min, barometer");
	set_colors(g, DOGRAPH_BAROMETER, meteoconfig);
	graph_label(g, mc_get_string_f(meteoconfig, "pressure", "left",
		interval, "label", "Pressure (hPa)"), 0);
	rangecolor = graph_color_allocate(g,
		mc_get_color(meteoconfig, "pressure.rangecolor",
			pressure_default_rangecolor));
	blue = graph_color_allocate(g,
		mc_get_color(meteoconfig, "pressure.color",
			pressure_default_color));
	graph_add_channel(g, GRAPH_HISTOGRAMM, rangecolor,
		mc_get_double_f(meteoconfig, "pressure", "left", interval,
			"min", PRESSUREMINDEFAULT),
		mc_get_double_f(meteoconfig, "pressure", "left", interval,
			"scale", PRESSURESCALEDEFAULT));
	graph_add_channel(g, GRAPH_HISTOGRAMM | GRAPH_HIDE, g->bg,
		mc_get_double_f(meteoconfig, "pressure", "left", interval,
			"min", PRESSUREMINDEFAULT),
		mc_get_double_f(meteoconfig, "pressure", "left", interval,
			"scale", PRESSURESCALEDEFAULT));
	graph_add_channel(g, GRAPH_LINE, blue,
		mc_get_double_f(meteoconfig, "pressure", "left", interval,
			"min", PRESSUREMINDEFAULT),
		mc_get_double_f(meteoconfig, "pressure", "left", interval,
			"scale", PRESSURESCALEDEFAULT));

	/* retrieve the data from the database				*/
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "query is '%s'", query);
	if (mysql_query(dgp->mysql, query)) {
		mdebug(LOG_CRIT, MDEBUG_LOG, 0, "cannot retrieve data: %s",
			mysql_error(dgp->mysql));
		exit(EXIT_FAILURE);
	}
	res = mysql_store_result(dgp->mysql);

	/* find out how many rows there are				*/
	nentries = mysql_num_rows(res);
	if (debug)
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
	graph_add_grid(g, 0,
		mc_get_double_f(meteoconfig, "pressure", "left", interval,
			"step", PRESSURESTEPDEFAULT),
		mc_get_double_f(meteoconfig, "pressure", "left", interval,
			"start", PRESSUREMINDEFAULT),
		mc_get_double_f(meteoconfig, "pressure", "left", interval,
			"end", PRESSUREMAXDEFAULT));
	graph_add_ticks(g, 0,
		mc_get_double_f(meteoconfig, "pressure", "left", interval,
			"step", PRESSURESTEPDEFAULT),
		mc_get_double_f(meteoconfig, "pressure", "left", interval,
			"start", PRESSUREMINDEFAULT),
		mc_get_double_f(meteoconfig, "pressure", "left", interval,
			"end", PRESSUREMAXDEFAULT),
		mc_get_string_f(meteoconfig, "pressure", "left", interval,
			"format", "%.0f"),
		0);

	/* compute the filename for the graph				*/
	create_filename(filename, sizeof(filename), dgp, "pressure");
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "pressure filename = %s",
			filename);
	graph_write_png(g, filename);
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "image written to %s",
			filename);
	
	/* free the SQL data						*/
	mysql_free_result(res);
	free(barodata);
	free(data);
}

/*
 * humidity graphs
 */
static color_t	humidity_default_color = { 100, 100, 255 };
static color_t	humidity_default_rangecolor = { 210, 210, 255 };

#define	HUMIDITYSTRING	(inside) ? "humidity_inside" : "humidity"
#define	HUMIDITYSTRING1(a)	(inside) ? ("humidity_inside." ## a)	\
					 : ("humidity." ## a)
#define	HUMIDITYSCALEDEFAULT	0.8333333
#define	HUMIDITYMINDEFAULT	0.
#define	HUMIDITYMAXDEFAULT	100.
#define	HUMIDITYSTEPDEFAULT	20.

static void	humidity_graphs_both(dograph_t *dgp, int interval, int inside) {
	graph_t		*g;
	char		query[2048], filename[1024];
	int		i, rangecolor, blue, nentries;
	entry_t		*data;
	double		*humiditydata;
	MYSQL_RES	*res;
	MYSQL_ROW	row;
	time_t		start;


	start = dgp->end;
	g = setup_graph(dgp, query, sizeof(query), interval, &start,
		(inside) ? "max(humidity_inside), min(humidity_inside), "
			   "avg(humidity_inside)"
		         : "max(humidity), min(humidity), avg(humidity)",
		(inside) ? "humidity_inside_max, humidity_inside_min, "
			   "humidity_inside"
			 : "humidity_max, humidity_min, humidity");
	set_colors(g, (inside) ? DOGRAPH_HUMIDITY_INSIDE : DOGRAPH_HUMIDITY,
		meteoconfig);
	graph_label(g, mc_get_string_f(meteoconfig, 
		HUMIDITYSTRING, "left",
		interval, "label", "rel. Humidity (%)"), 0);
	rangecolor = graph_color_allocate(g,
		mc_get_color(meteoconfig, HUMIDITYSTRING1("rangecolor"),
			humidity_default_rangecolor));
	blue = graph_color_allocate(g,
		mc_get_color(meteoconfig, HUMIDITYSTRING1("color"),
			humidity_default_color));
	graph_add_channel(g, GRAPH_HISTOGRAMM, rangecolor,
		mc_get_double_f(meteoconfig, HUMIDITYSTRING, "left", interval,
			"min", HUMIDITYMINDEFAULT),
		mc_get_double_f(meteoconfig, HUMIDITYSTRING, "left", interval,
			"scale", HUMIDITYSCALEDEFAULT));
	graph_add_channel(g, GRAPH_HISTOGRAMM | GRAPH_HIDE, g->bg,
		mc_get_double_f(meteoconfig, HUMIDITYSTRING, "left", interval,
			"min", HUMIDITYMINDEFAULT),
		mc_get_double_f(meteoconfig, HUMIDITYSTRING, "left", interval,
			"scale", HUMIDITYSCALEDEFAULT));
	graph_add_channel(g, GRAPH_LINE, blue,
		mc_get_double_f(meteoconfig, HUMIDITYSTRING, "left", interval,
			"min", HUMIDITYMINDEFAULT),
		mc_get_double_f(meteoconfig, HUMIDITYSTRING, "left", interval,
			"scale", HUMIDITYSCALEDEFAULT));

	/* retrieve the data from the database				*/
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "query is '%s'", query);
	if (mysql_query(dgp->mysql, query)) {
		mdebug(LOG_CRIT, MDEBUG_LOG, 0, "cannot retrieve data: %s",
			mysql_error(dgp->mysql));
		exit(EXIT_FAILURE);
	}
	res = mysql_store_result(dgp->mysql);

	/* find out how many rows there are				*/
	nentries = mysql_num_rows(res);
	if (debug)
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
	graph_add_grid(g, 0,
		mc_get_double_f(meteoconfig, HUMIDITYSTRING, "left", interval,
			"step", HUMIDITYSTEPDEFAULT),
		mc_get_double_f(meteoconfig, HUMIDITYSTRING, "left", interval,
			"start", HUMIDITYMINDEFAULT),
		mc_get_double_f(meteoconfig, HUMIDITYSTRING, "left", interval,
			"end", HUMIDITYMAXDEFAULT));
	graph_add_ticks(g, 0,
		mc_get_double_f(meteoconfig, HUMIDITYSTRING, "left", interval,
			"step", HUMIDITYSTEPDEFAULT),
		mc_get_double_f(meteoconfig, HUMIDITYSTRING, "left", interval,
			"start", HUMIDITYMINDEFAULT),
		mc_get_double_f(meteoconfig, HUMIDITYSTRING, "left", interval,
			"end", HUMIDITYMAXDEFAULT),
		mc_get_string_f(meteoconfig, HUMIDITYSTRING, "left", interval,
			"format", "%.0f"),
		0);

	/* compute the filename for the graph				*/
	create_filename(filename, sizeof(filename), dgp, HUMIDITYSTRING);
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "humidity filename = %s",
			filename);
	graph_write_png(g, filename);
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "image written to %s",
			filename);
	
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
static color_t	temp_default_color = { 180, 0, 0 };
static color_t	temp_default_rangecolor = { 255, 180, 180 };
static color_t	temp_default_dewcolor = { 100, 100, 255 };

#define	TEMPERATURESCALEDEFAULT	.5
#define	TEMPERATUREMINDEFAULT	-10.
#define	TEMPERATUREMAXDEFAULT	30.
#define	TEMPERATURESTEPDEFAULT	10.

#define	TEMPERATURESTRING	(inside) ? "temperature_inside" : "temperature"
#define	TEMPERATURESTRING1(a)	(inside) ? ("temperature_inside" ## a) \
					 : ("temperature" ## a)
static void	temp_graphs_both(dograph_t *dgp, int interval, int inside) {
	time_t		start;
	graph_t		*g;
	char		query[2048], filename[1024];
	int		i, rangecolor, color, dewcolor, nentries;
	entry_t		*data;
	double		*tempdata;
	MYSQL_RES	*res;
	MYSQL_ROW	row;

	start = dgp->end;
	/* setup the temperature graph, depending on inside flag	*/
	if (inside) {
		g = setup_graph(dgp, query, sizeof(query), interval,
			&start,
			"min(temperature_inside), "
			"max(temperature_inside), "
			"avg(temperature_inside), "
			"avg(humidity_inside)",
			"temperature_inside_min, "
			"temperature_inside_max, "
			"temperature_inside, "
			"humidity_inside");
		set_colors(g, DOGRAPH_TEMPERATURE_INSIDE, meteoconfig);
	} else {
		g = setup_graph(dgp, query, sizeof(query), interval,
			&start,
			"ifnull(min(temperature), -273.), "
			"ifnull(max(temperature), 100.), "
			"ifnull(avg(temperature), -273.), "
			"ifnull(avg(humidity), 0.)",
			"temperature_min, "
			"temperature_max, "
			"temperature, "
			"humidity");
		set_colors(g, DOGRAPH_TEMPERATURE, meteoconfig);
	}
	
	/* adornments of the temperature graph				*/
	graph_label(g, mc_get_string_f(meteoconfig, TEMPERATURESTRING,
			"left", interval, "label",
			"Temperature (deg C)"), 0);
	rangecolor = graph_color_allocate(g,
		mc_get_color(meteoconfig, TEMPERATURESTRING1(".rangecolor"),
			temp_default_rangecolor));
	color = graph_color_allocate(g,
		mc_get_color(meteoconfig, TEMPERATURESTRING1(".color"),
			temp_default_color));
	dewcolor = graph_color_allocate(g,
		mc_get_color(meteoconfig, TEMPERATURESTRING1(".dewcolor"),
			temp_default_dewcolor));
	graph_add_channel(g, GRAPH_HISTOGRAMM, rangecolor,
		mc_get_double_f(meteoconfig, TEMPERATURESTRING,
			"left", interval, "min", -15.),
		mc_get_double_f(meteoconfig, TEMPERATURESTRING,
			"left", interval, "scale", .5));
	graph_add_channel(g, GRAPH_HISTOGRAMM | GRAPH_HIDE, g->bg,
		mc_get_double_f(meteoconfig, TEMPERATURESTRING,
			"left", interval, "min",-15.),
		mc_get_double_f(meteoconfig, TEMPERATURESTRING,
			"left", interval, "scale", .5));
	graph_add_channel(g, GRAPH_LINE, dewcolor,
		mc_get_double_f(meteoconfig, TEMPERATURESTRING,
			"left", interval, "min", -15.),
		mc_get_double_f(meteoconfig, TEMPERATURESTRING,
			"left", interval, "scale", .5));
	graph_add_channel(g, GRAPH_LINE, color,
		mc_get_double_f(meteoconfig, TEMPERATURESTRING,
			"left", interval, "min", -15.),
		mc_get_double_f(meteoconfig, TEMPERATURESTRING,
			"left", interval, "scale", .5));

	/* retrieve the data from the database				*/
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "query is '%s'", query);
	if (mysql_query(dgp->mysql, query)) {
		mdebug(LOG_CRIT, MDEBUG_LOG, 0, "cannot retrieve data: %s",
			mysql_error(dgp->mysql));
		exit(EXIT_FAILURE);
	}
	res = mysql_store_result(dgp->mysql);

	/* find out how many rows there are				*/
	nentries = mysql_num_rows(res);
	if (debug)
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
	graph_add_grid(g, 0,
		mc_get_double_f(meteoconfig,
			TEMPERATURESTRING,
			"left", interval, "step", TEMPERATURESTEPDEFAULT),
		mc_get_double_f(meteoconfig,
			TEMPERATURESTRING,
			"left", interval, "sart", TEMPERATUREMINDEFAULT),
		mc_get_double_f(meteoconfig,
			TEMPERATURESTRING,
			"left", interval, "end", TEMPERATUREMAXDEFAULT));
	graph_add_ticks(g, 0,
		mc_get_double_f(meteoconfig,
			TEMPERATURESTRING,
			"left", interval, "step", TEMPERATURESTEPDEFAULT),
		mc_get_double_f(meteoconfig,
			TEMPERATURESTRING,
			"left", interval, "start", TEMPERATUREMINDEFAULT),
		mc_get_double_f(meteoconfig,
			TEMPERATURESTRING,
			"left", interval, "end", TEMPERATUREMAXDEFAULT),
		mc_get_string_f(meteoconfig,
			TEMPERATURESTRING,
			"left", interval, "format", "%.0f"),
		0);

	/* compute the filename for the graph				*/
	create_filename(filename, sizeof(filename), dgp, TEMPERATURESTRING);
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "temperature filename = %s",
			filename);
	graph_write_png(g, filename);
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "image written to %s",
			filename);
	
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
static color_t	rain_default_color = { 0, 0, 255 };

#define	RAINDEFAULTMIN	0.
#define	RAINDEFAULTMAX	30.
#define	RAINDEFAULTSCALE	0.01
#define	RAINDEFAULTSTEP	1.

void	rain_graphs(dograph_t *dgp, int interval) {
	time_t		start;
	graph_t		*g;
	char		query[2048], filename[1024];
	int		i, color, nentries;
	entry_t		*data;
	double		*tempdata;
	MYSQL_RES	*res;
	MYSQL_ROW	row;

	/* set up the rain query					*/
	start = dgp->end;
	g = setup_graph(dgp, query, sizeof(query), interval, &start,
		"sum(rain)", "rain");
	set_colors(g, DOGRAPH_RAIN, meteoconfig);
	graph_label(g,
		mc_get_string_f(meteoconfig, "rain", "left", interval, "label",
			"Precipitation (mm)"), 0);

	color = graph_color_allocate(g,
		mc_get_color(meteoconfig, "rain.color", rain_default_color));
	graph_add_channel(g, GRAPH_HISTOGRAMM, color, 
		mc_get_double_f(meteoconfig, "rain", "left", interval,
			"min", RAINDEFAULTMIN),
		mc_get_double_f(meteoconfig, "rain", "left", interval,
			"scale", RAINDEFAULTSCALE));

	/* retrieve the data from the database				*/
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "query is '%s'", query);
	if (mysql_query(dgp->mysql, query)) {
		mdebug(LOG_CRIT, MDEBUG_LOG, 0, "cannot retrieve data: %s",
			mysql_error(dgp->mysql));
		exit(EXIT_FAILURE);
	}
	res = mysql_store_result(dgp->mysql);

	/* find out how many rows there are				*/
	nentries = mysql_num_rows(res);
	if (debug)
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
	graph_add_grid(g, 0,
		mc_get_double_f(meteoconfig, "rain", "left", interval,
			"step", RAINDEFAULTSTEP),
		mc_get_double_f(meteoconfig, "rain", "left", interval,
			"start", RAINDEFAULTMIN),
		mc_get_double_f(meteoconfig, "rain", "left", interval,
			"end", RAINDEFAULTMAX));
	graph_add_ticks(g, 0,
		mc_get_double_f(meteoconfig, "rain", "left", interval,
			"step", RAINDEFAULTSTEP),
		mc_get_double_f(meteoconfig, "rain", "left", interval,
			"start", RAINDEFAULTMIN),
		mc_get_double_f(meteoconfig, "rain", "left", interval,
			"end", RAINDEFAULTMAX),
		mc_get_string_f(meteoconfig, "rain", "left", interval,
			"format", "%.1f"),
		0);

	/* compute the filename for the graph				*/
	create_filename(filename, sizeof(filename), dgp, "rain");
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "rain filename = %s",
			filename);
	graph_write_png(g, filename);
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "image written to %s",
			filename);
	
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

static void wind_background(graph_t *g, int interval) {
	const int	*coln, *cols, *colw, *cole;
	int	dc;
	/* check for north color					*/
	coln = mc_get_color_f(meteoconfig, "wind", "right", interval,
			"northcolor", NULL);
	if (coln != NULL) {
		dc = gdImageColorAllocate(g->im, coln[0], coln[1], coln[2]);
		wind_rectangle(g, 0., 45., dc);
		wind_rectangle(g, 315., 360., dc);
	}
	/* check for the west color					*/
	colw = mc_get_color_f(meteoconfig, "wind", "right", interval,
			"westcolor", NULL);
	if (colw != NULL) {
		dc = gdImageColorAllocate(g->im, colw[0], colw[1], colw[2]);
		wind_rectangle(g, 225., 315., dc);
	}
	/* check for the east color					*/
	cole = mc_get_color_f(meteoconfig, "wind", "right", interval,
			"eastcolor", NULL);
	if (cole != NULL) {
		dc = gdImageColorAllocate(g->im, cole[0], cole[1], cole[2]);
		wind_rectangle(g, 45., 135., dc);
	}
	/* check for the south color					*/
	cols = mc_get_color_f(meteoconfig, "wind", "right", interval,
			"southcolor", NULL);
	if (cols != NULL) {
		dc = gdImageColorAllocate(g->im, cols[0], cols[1], cols[2]);
		wind_rectangle(g, 135., 225., dc);
	}
}

static color_t	wind_default_color = { 100, 100, 255 };
static color_t	wind_default_gustcolor = { 0, 100, 0 };
static color_t	wind_default_speedcolor = { 0, 255, 0 };

#define	WINDSPEEDDEFAULTSCALE	0.25
#define	WINDSPEEDDEFAULTMIN	0.
#define	WINDSPEEDDEFAULTMAX	10.
#define	WINDSPEEDDEFAULTSTART	0.
#define	WINDSPEEDDEFAULTEND	10.
#define	WINDSPEEDDEFAULTSTEP	5.

#define	WINDDIRDEFAULTSCALE	8.
#define	WINDDIRDEFAULTSTART	0.
#define	WINDDIRDEFAULTEND	100.
#define	WINDDIRDEFAULTMIN	-400.
#define	WINDDIRDEFAULTSTEP	180.

void	wind_graphs(dograph_t *dgp, int interval) {
	time_t		start;
	graph_t		*g;
	char		query[2048], filename[1024];
	int		i, speedcolor, gustcolor, dircolor, nentries;
	entry_t		*data;
	double		*winddata, x, y;
	MYSQL_RES	*res;
	MYSQL_ROW	row;

	start = dgp->end;
	g = setup_graph(dgp, query, sizeof(query), interval, &start,
		"sum(windx), sum(windy), sum(duration), "
		"max(windgust)",
		"windgust, windspeed, winddir");
	set_colors(g, DOGRAPH_WIND, meteoconfig);
	graph_label(g, mc_get_string_f(meteoconfig, "wind", "left", interval,
		"label", "Speed (m/s)"), 0);
	graph_label(g, mc_get_string_f(meteoconfig, "wind", "right", interval,
		"label", "Azimut (deg)"), 1);

	dircolor = graph_color_allocate(g, mc_get_color(meteoconfig,
		"wind.color", wind_default_color));
	gustcolor = graph_color_allocate(g, mc_get_color(meteoconfig,
		"wind.gustcolor", wind_default_gustcolor));
	speedcolor = graph_color_allocate(g, mc_get_color(meteoconfig,
		"wind.speedcolor", wind_default_speedcolor));
	graph_add_channel(g, GRAPH_HISTOGRAMM, gustcolor,
		mc_get_double_f(meteoconfig, "wind", "left", interval,
			"min", WINDSPEEDDEFAULTMIN),
		mc_get_double_f(meteoconfig, "wind", "left", interval,
			"scale", WINDSPEEDDEFAULTSCALE));
	graph_add_channel(g, GRAPH_HISTOGRAMM, speedcolor,
		mc_get_double_f(meteoconfig, "wind", "left", interval,
			"min", WINDSPEEDDEFAULTMIN),
		mc_get_double_f(meteoconfig, "wind", "left", interval,
			"scale", WINDSPEEDDEFAULTSCALE));
	graph_add_channel(g, GRAPH_LINE, dircolor,
		mc_get_double_f(meteoconfig, "wind", "right", interval,
			"min", WINDDIRDEFAULTMIN),
		mc_get_double_f(meteoconfig, "wind", "right", interval,
			"scale", WINDDIRDEFAULTSCALE));

	/* retrieve the data from the database				*/
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "query is '%s'", query);
	if (mysql_query(dgp->mysql, query)) {
		mdebug(LOG_CRIT, MDEBUG_LOG, 0, "cannot retrieve data: %s",
			mysql_error(dgp->mysql));
		exit(EXIT_FAILURE);
	}
	res = mysql_store_result(dgp->mysql);

	/* find out how many rows there are				*/
	nentries = mysql_num_rows(res);
	if (debug)
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
	wind_background(g, interval);

	/* display the data retrieved from the database			*/
	graph_add_data(g, start, interval, nentries, data);

	/* display the grid						*/
	graph_add_time(g);
	graph_add_grid(g, 0,
		mc_get_double_f(meteoconfig, "wind", "left", interval,
			"step", WINDSPEEDDEFAULTSTEP),
		mc_get_double_f(meteoconfig, "wind", "left", interval,
			"start", WINDSPEEDDEFAULTSTART),
		mc_get_double_f(meteoconfig, "wind", "left", interval,
			"end", WINDSPEEDDEFAULTEND));
	graph_add_grid(g, 2,
		mc_get_double_f(meteoconfig, "wind", "right", interval,
			"step", WINDDIRDEFAULTSTEP),
		mc_get_double_f(meteoconfig, "wind", "right", interval,
			"start", WINDDIRDEFAULTSTART),
		mc_get_double_f(meteoconfig, "wind", "right", interval,
			"end", WINDDIRDEFAULTEND));
	graph_add_ticks(g,
		0,
		mc_get_double_f(meteoconfig, "wind", "left", interval,
			"step", WINDSPEEDDEFAULTSTEP),
		mc_get_double_f(meteoconfig, "wind", "left", interval,
			"start", WINDSPEEDDEFAULTSTART),
		mc_get_double_f(meteoconfig, "wind", "left", interval,
			"end", WINDSPEEDDEFAULTEND),
		mc_get_string_f(meteoconfig, "wind", "left", interval,
			"format", "%.0f"),
		0);
	graph_add_ticks(g,
		2,
		mc_get_double_f(meteoconfig, "wind", "right", interval,
			"step", WINDDIRDEFAULTSTEP),
		mc_get_double_f(meteoconfig, "wind", "right", interval,
			"start", WINDDIRDEFAULTSTART),
		mc_get_double_f(meteoconfig, "wind", "right", interval,
			"end", WINDDIRDEFAULTEND),
		mc_get_string_f(meteoconfig, "wind", "right", interval,
			"format", "%.0f"),
		1);

	/* compute the filename for the graph				*/
	create_filename(filename, sizeof(filename), dgp, "wind");
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "wind filename = %s",
			filename);
	graph_write_png(g, filename);
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "image written to %s",
			filename);
}

/*
 * radiation graphs
 */
static color_t	radiation_default_solar_color = { 200, 180, 0 };
static color_t	radiation_default_uv_color = { 100, 0, 200 };

void	radiation_graphs(dograph_t *dgp, int interval) {
	time_t		start;
	graph_t		*g;
	char		query[2048], filename[1024];
	int		i, solarcolor, uvcolor, nentries;
	entry_t		*data;
	double		*radiationdata;
	MYSQL_RES	*res;
	MYSQL_ROW	row;

	start = dgp->end;
	g = setup_graph(dgp, query, sizeof(query), interval, &start,
		"avg(solar), avg(uv)",
		"solar, uv");
	set_colors(g, DOGRAPH_RADIATION, meteoconfig);
	graph_label(g, mc_get_string_f(meteoconfig, "radiation", "left",
		interval, "label", "Solar Radiation (W/m2)"), 0);
	graph_label(g, mc_get_string_f(meteoconfig, "radiation", "right",
		interval, "label", "UV index"), 1);

	solarcolor = graph_color_allocate(g, mc_get_color(meteoconfig,
		"radiation.solarcolor", radiation_default_solar_color));
	uvcolor = graph_color_allocate(g, mc_get_color(meteoconfig,
		"radiation.uvcolor", radiation_default_uv_color));
	graph_add_channel(g, GRAPH_HISTOGRAMM, solarcolor,
		mc_get_double_f(meteoconfig, "radiation", "left",
			interval, "min", 0.),
		mc_get_double_f(meteoconfig, "radiation", "left",
			interval, "scale", 5));
	graph_add_channel(g, GRAPH_LINE, uvcolor,
		mc_get_double_f(meteoconfig, "radiation", "right",
			interval, "min", 0.),
		mc_get_double_f(meteoconfig, "radiation", "right",
			interval, "scale", 0.05));

	/* retrieve the data from the database				*/
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "query is '%s'", query);
	if (mysql_query(dgp->mysql, query)) {
		mdebug(LOG_CRIT, MDEBUG_LOG, 0, "cannot retrieve data: %s",
			mysql_error(dgp->mysql));
		exit(EXIT_FAILURE);
	}
	res = mysql_store_result(dgp->mysql);

	/* find out how many rows there are				*/
	nentries = mysql_num_rows(res);
	if (debug)
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
	graph_add_grid(g, 0,
		mc_get_double_f(meteoconfig, "radiation", "left", interval,
			"step", 100.),
		mc_get_double_f(meteoconfig, "radiation", "left", interval,
			"start", 0.),
		mc_get_double_f(meteoconfig, "radiation", "left", interval,
			"end", 600.));
	/* it's not necessary to write the grid lines twice...
	graph_add_grid(g, 1,
		mc_get_double_f(meteoconfig, "radiation", "right", interval,
			"step", 1.),
		mc_get_double_f(meteoconfig, "radiation", "right", interval,
			"start", 0.),
		mc_get_double_f(meteoconfig, "radiation", "right", interval,
			"end", 6.));
	*/
	graph_add_ticks(g,
		0,
		mc_get_double_f(meteoconfig, "radiation", "left", interval,
			"step", 100.),
		mc_get_double_f(meteoconfig, "radiation", "left", interval,
			"start", 0.),
		mc_get_double_f(meteoconfig, "radiation", "left", interval,
			"end", 600.),
		mc_get_string_f(meteoconfig, "radiation", "left", interval,
			"format", "%.0f"),
		0);
	graph_add_ticks(g,
		1,
		mc_get_double_f(meteoconfig, "radiation", "right", interval,
			"step", 1.),
		mc_get_double_f(meteoconfig, "radiation", "right", interval,
			"start", 0.),
		mc_get_double_f(meteoconfig, "radiation", "right", interval,
			"end", 6.),
		mc_get_string_f(meteoconfig, "radiation", "right", interval,
			"format", "%.1f"),
		1);

	/* compute the filename for the graph				*/
	create_filename(filename, sizeof(filename), dgp, "radiation");
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "radiation filename = %s",
			filename);
	graph_write_png(g, filename);
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "image written to %s",
			filename);
}

/*
 * all graphs
 */
void	all_graphs(dograph_t *dgp, int interval) {
	if (debug)
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

