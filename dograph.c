/*
 * dograph.c -- create meteo data graphs similar to MRTGs
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: dograph.c,v 1.1 2001/12/26 22:10:45 afm Exp $
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
#include <errno.h>
#include <time.h>
#include <timestamp.h>
#include <dograph.h>

#define	stampformat	"%Y%m%d%H%M%S"
graph_t 	*setup_graph(const dograph_t *dgp, char *query, int querylen,
	int interval, time_t *start, char *data, char *avgdata) {
	graph_t	*g;
	char	startstamp[32], endstamp[32];

	/* set up the graph geometrically				*/
	g = graph_new(dgp->prefix, METEO_WIDTH, METEO_HEIGHT);
	graph_set_dimensions(g, METEO_LLX, METEO_LLY, METEO_URX, METEO_URY);

	/* set up the time margins for the graph			*/
	*start -= *start % interval;
	strftime(endstamp, 32, stampformat, localtime(start));
	*start -= 400 * interval;
	strftime(startstamp, 32, stampformat, localtime(start));

	/* formulate the query						*/
	if (dgp->useaverages)
		snprintf(query, querylen,
			"select unix_timestamp(timekey) - %d, %s "
			"from averages "
			"where timekey between  %s and %s "
			"  and station = '%-8.8s' "
			"  and intval = %d "
			"limit 400",
			interval, avgdata, startstamp, endstamp,
			dgp->prefix, interval);
	else
		snprintf(query, querylen,
			"select group%d * %d, %s "
			"from stationdata "
			"where timekey between  %s and %s "
			"  and station = '%-8.8s' "
			"group by group%d "
			"order by 1 "
			"limit 400",
			interval, interval, data, startstamp, endstamp,
			dgp->prefix, interval);

	return g;
}

/*
 * barometer graphs
 */
static color_t	pressure_default_color = { 127, 127, 255 };
static color_t	pressure_default_rangecolor = { 210, 210, 255 };

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
	graph_label(g, mc_get_string(meteoconfig, "pressure.left.label",
		"Pressure (hPa)"), 0);
	rangecolor = graph_color_allocate(g,
		mc_get_color(meteoconfig, "pressure.rangecolor",
			pressure_default_rangecolor));
	blue = graph_color_allocate(g,
		mc_get_color(meteoconfig, "pressure.color",
			pressure_default_color));
	graph_add_channel(g, GRAPH_HISTOGRAMM, rangecolor,
		mc_get_double(meteoconfig, "pressure.left.min", 930.),
		mc_get_double(meteoconfig, "pressure.left.scale", 0.5));
	graph_add_channel(g, GRAPH_HISTOGRAMM | GRAPH_HIDE, g->bg,
		mc_get_double(meteoconfig, "pressure.left.min", 930.),
		mc_get_double(meteoconfig, "pressure.left.scale", 0.5));
	graph_add_channel(g, GRAPH_LINE, blue,
		mc_get_double(meteoconfig, "pressure.left.min", 930.),
		mc_get_double(meteoconfig, "pressure.left.scale", 0.5));

	/* retrieve the data from the database				*/
	if (debug)
		fprintf(stderr, "%s:%d: query is '%s'\n", __FILE__, __LINE__,
			query);
	if (mysql_query(dgp->mysql, query)) {
		fprintf(stderr, "%s:%d: cannot retrieve data\n", __FILE__,
			__LINE__);
		exit(EXIT_FAILURE);
	}
	res = mysql_store_result(dgp->mysql);

	/* find out how many rows there are				*/
	nentries = mysql_num_rows(res);
	if (debug)
		fprintf(stderr, "%s:%d: baro query returns %d rows\n",
			__FILE__, __LINE__, nentries);

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
			fprintf(stderr, "%s:%d: %9d %6.1f %6.1f %6.1f\n",
				__FILE__, __LINE__, (int)data[i].when,
				data[i].data[0], data[i].data[1],
				data[i].data[2]);
	}

	/* display the data retrieved from the database			*/
	graph_add_data(g, start, interval, nentries, data);

	/* display the grid						*/
	graph_add_time(g);
	graph_add_grid(g, 0,
		mc_get_double(meteoconfig, "pressure.left.step", 10.),
		mc_get_double(meteoconfig, "pressure.left.start", 930.),
		mc_get_double(meteoconfig, "pressure.left.end", 980.));
	graph_add_ticks(g, 0,
		mc_get_double(meteoconfig, "pressure.left.step", 10.),
		mc_get_double(meteoconfig, "pressure.left.start", 930.),
		mc_get_double(meteoconfig, "pressure.left.end", 980.),
		mc_get_string(meteoconfig, "pressure.left.format", "%.0f"),
		0);

	/* compute the filename for the graph				*/
	snprintf(filename, sizeof(filename), "%s-barometer-%s.png", dgp->prefix,
		dgp->suffix);
	graph_write_png(g, filename);
	if (debug)
		fprintf(stderr, "%s:%d: image written to %s\n", __FILE__,
			__LINE__, filename);
	
	/* free the SQL data						*/
	mysql_free_result(res);
	free(barodata);
	free(data);
}

/*
 * temperature graphs
 */
static color_t	temp_default_color = { 180, 0, 0 };
static color_t	temp_default_rangecolor = { 255, 180, 180 };
static color_t	temp_default_dewcolor = { 100, 100, 255 };
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
	if (inside)
		g = setup_graph(dgp, query, sizeof(query), interval,
			&start,
			"min(temperature_inside), "
			"max(temperature_inside), avg(temperature_inside), "
			"avg(humidity_inside)",
			"temperature_inside_min, temperature_inside_max, "
			"temperature_inside, humidity_inside");
	else
		g = setup_graph(dgp, query, sizeof(query), interval,
			&start,
			"min(temperature), max(temperature), avg(temperature), "
			"avg(humidity)",
			"temperature_min, temperature_max, temperature, "
			"humidity");
	
	/* adornments of the temperature graph				*/
	graph_label(g, mc_get_string(meteoconfig,
			(inside)	? "temperature_inside.left.label"
					: "temperature.left.label",
			"Temperature (deg C)"), 0);
	rangecolor = graph_color_allocate(g,
		mc_get_color(meteoconfig,
			(inside)	? "temperature_inside.rangecolor"
					: "temperature.rangecolor",
			temp_default_rangecolor));
	color = graph_color_allocate(g,
		mc_get_color(meteoconfig,
			(inside)	? "temperature_inside.color"
					: "temperature.color",
			temp_default_color));
	dewcolor = graph_color_allocate(g,
		mc_get_color(meteoconfig,
			(inside)	? "temperature_inside.dewcolor"
					: "temperature.dewcolor",
			temp_default_dewcolor));
	graph_add_channel(g, GRAPH_HISTOGRAMM, rangecolor,
		mc_get_double(meteoconfig,
			(inside)	? "temperature_inside.left.min"
					: "temperature.left.min", -15.),
		mc_get_double(meteoconfig,
			(inside)	? "temperature_inside.left.scale"
					: "temperature.left.scale", .5));
	graph_add_channel(g, GRAPH_HISTOGRAMM | GRAPH_HIDE, g->bg,
		mc_get_double(meteoconfig,
			(inside)	? "temperature_inside.left.min"
					: "temperature.left.min", -15.),
		mc_get_double(meteoconfig,
			(inside)	? "temperature_inside.left.scale"
					: "temperature.left.scale", .5));
	graph_add_channel(g, GRAPH_LINE, dewcolor,
		mc_get_double(meteoconfig,
			(inside)	? "temperature_inside.left.min"
					: "temperature.left.min", -15.),
		mc_get_double(meteoconfig,
			(inside)	? "temperature_inside.left.scale"
					: "temperature.left.scale", .5));
	graph_add_channel(g, GRAPH_LINE, color,
		mc_get_double(meteoconfig,
			(inside)	? "temperature_inside.left.min"
					: "temperature.left.min", -15.),
		mc_get_double(meteoconfig,
			(inside)	? "temperature_inside.left.scale"
					: "temperature.left.scale", .5));

	/* retrieve the data from the database				*/
	if (debug)
		fprintf(stderr, "%s:%d: query is '%s'\n", __FILE__, __LINE__,
			query);
	if (mysql_query(dgp->mysql, query)) {
		fprintf(stderr, "%s:%d: cannot retrieve data\n", __FILE__,
			__LINE__);
		exit(EXIT_FAILURE);
	}
	res = mysql_store_result(dgp->mysql);

	/* find out how many rows there are				*/
	nentries = mysql_num_rows(res);
	if (debug)
		fprintf(stderr, "%s:%d: temp query returns %d rows\n",
			__FILE__, __LINE__, nentries);

	/* allocate memory to store all the data			*/
	data = (entry_t *)malloc(sizeof(entry_t) * nentries);
	tempdata = (double *)malloc(sizeof(double) * nentries * 4);

	/* read row by row and store in the data array			*/
	for (i = 0; i < nentries; i++) {
		row = mysql_fetch_row(res);
		data[i].when = atoi(row[0]);
		data[i].data = &tempdata[i * 4];
		data[i].data[0] = atof(row[2]);	/* max temperature	*/
		data[i].data[1] = atof(row[1]);	/* min temeprature	*/
		data[i].data[3] = atof(row[3]);
		/* compute the dew point based on temperature/humidity	*/
		data[i].data[2] = atof(row[4]);
		if (data[i].data[2] > 100.) data[i].data[2] = 100.;
		data[i].data[2] = dewpoint(data[i].data[2], data[i].data[3]);
		if (debug > 1)
			fprintf(stderr, "%s:%d: %9d %6.1f %6.1f %6.1f\n",
				__FILE__, __LINE__, (int)data[i].when,
				data[i].data[0], data[i].data[1],
				data[i].data[2]);
	}

	/* display the data retrieved from the database			*/
	graph_add_data(g, start, interval, nentries, data);

	/* display the grid						*/
	graph_add_time(g);
	graph_add_grid(g, 0,
		mc_get_double(meteoconfig,
			(inside)	? "temperature_inside.left.step"
					: "temperature.left.step", 10.),
		mc_get_double(meteoconfig,
			(inside)	? "temperature_inside.left.start"
					: "temperature.left.start", -10.),
		mc_get_double(meteoconfig,
			(inside)	? "temperature_inside.left.end"
					: "temperature.left.end", 30.));
	graph_add_ticks(g, 0,
		mc_get_double(meteoconfig,
			(inside)	? "temperature_inside.left.step"
					: "temperature.left.step", 10.),
		mc_get_double(meteoconfig,
			(inside)	? "temperature_inside.left.start"
					: "temperature.left.start", -10.),
		mc_get_double(meteoconfig,
			(inside)	? "temperature_inside.left.end"
					: "temperature.left.end", 30.),
		mc_get_string(meteoconfig,
			(inside)	? "temperature_inside.left.format"
					: "temperature.left.format", "%.0f"),
		0);

	/* compute the filename for the graph				*/
	snprintf(filename, sizeof(filename),
		(inside)	? "%s-temperature_inside-%s.png"
				: "%s-temperature-%s.png",
		dgp->prefix, dgp->suffix);
	graph_write_png(g, filename);
	if (debug)
		fprintf(stderr, "%s:%d: image written to %s\n", __FILE__,
			__LINE__, filename);
	
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

void	rain_graphs(dograph_t *dgp, int interval) {
	time_t		start;
	graph_t		*g;
	char		query[2048], filename[1024];
	char		scalekey[64], unitkey[64], minkey[64], startkey[64],
			endkey[64], formatkey[64];
	int		i, color, nentries;
	entry_t		*data;
	double		*tempdata;
	MYSQL_RES	*res;
	MYSQL_ROW	row;

	/* set up the rain query					*/
	start = dgp->end;
	g = setup_graph(dgp, query, sizeof(query), interval, &start,
		"sum(rain)", "rain");
	graph_label(g,
		mc_get_string(meteoconfig, "rain.left.label",
			"Precipitation (mm)"), 0);

	snprintf(scalekey, sizeof(scalekey), "rain.left.%d.scale", interval);
	snprintf(unitkey, sizeof(unitkey), "rain.left.%d.step", interval);
	snprintf(minkey, sizeof(minkey), "rain.left.%d.min", interval);
	snprintf(startkey, sizeof(startkey), "rain.left.%d.start", interval);
	snprintf(endkey, sizeof(endkey), "rain.left.%d.end", interval);
	snprintf(formatkey, sizeof(formatkey), "rain.left.%d.format", interval);

	color = graph_color_allocate(g,
		mc_get_color(meteoconfig, "rain.color", rain_default_color));
	graph_add_channel(g, GRAPH_HISTOGRAMM, color, 
		mc_get_double(meteoconfig, minkey, 0.),
		mc_get_double(meteoconfig, scalekey, .01));

	/* retrieve the data from the database				*/
	if (debug)
		fprintf(stderr, "%s:%d: query is '%s'\n", __FILE__, __LINE__,
			query);
	if (mysql_query(dgp->mysql, query)) {
		fprintf(stderr, "%s:%d: cannot retrieve data\n", __FILE__,
			__LINE__);
		exit(EXIT_FAILURE);
	}
	res = mysql_store_result(dgp->mysql);

	/* find out how many rows there are				*/
	nentries = mysql_num_rows(res);
	if (debug)
		fprintf(stderr, "%s:%d: temp query returns %d rows\n",
			__FILE__, __LINE__, nentries);

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
			fprintf(stderr, "%s:%d: %9d %6.1f\n",
				__FILE__, __LINE__, (int)data[i].when,
				data[i].data[0]);
	}

	/* display the data retrieved from the database			*/
	graph_add_data(g, start, interval, nentries, data);

	/* display the grid						*/
	graph_add_time(g);
	graph_add_grid(g, 0,
		mc_get_double(meteoconfig, unitkey, 1.),
		mc_get_double(meteoconfig, startkey, 0.),
		mc_get_double(meteoconfig, endkey, 30.));
	graph_add_ticks(g, 0,
		mc_get_double(meteoconfig, unitkey, 1.),
		mc_get_double(meteoconfig, startkey, 0.),
		mc_get_double(meteoconfig, endkey, 30.),
		mc_get_string(meteoconfig, formatkey, "%.1f"),
		0);

	/* compute the filename for the graph				*/
	snprintf(filename, sizeof(filename), "%s-rain-%s.png",
		dgp->prefix, dgp->suffix);
	graph_write_png(g, filename);
	if (debug)
		fprintf(stderr, "%s:%d: image written to %s\n", __FILE__,
			__LINE__, filename);
	
	/* free the SQL data						*/
	mysql_free_result(res);
	free(tempdata);
	free(data);
}

static color_t	wind_default_color = { 100, 100, 255 };
static color_t	wind_default_gustcolor = { 0, 100, 0 };
static color_t	wind_default_speedcolor = { 0, 255, 0 };

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
	graph_label(g, mc_get_string(meteoconfig, "wind.left.label",
		"Speed (m/s)"), 0);
	graph_label(g, mc_get_string(meteoconfig, "wind.right.label",
		"Azimut (deg)"), 1);

	dircolor = graph_color_allocate(g, mc_get_color(meteoconfig,
		"wind.color", wind_default_color));
	gustcolor = graph_color_allocate(g, mc_get_color(meteoconfig,
		"wind.gustcolor", wind_default_gustcolor));
	speedcolor = graph_color_allocate(g, mc_get_color(meteoconfig,
		"wind.speedcolor", wind_default_speedcolor));
	graph_add_channel(g, GRAPH_HISTOGRAMM, gustcolor,
		mc_get_double(meteoconfig, "wind.left.min", 0.),
		mc_get_double(meteoconfig, "wind.left.scale", .25));
	graph_add_channel(g, GRAPH_HISTOGRAMM, speedcolor,
		mc_get_double(meteoconfig, "wind.left.min", 0.),
		mc_get_double(meteoconfig, "wind.left.scale", .25));
	graph_add_channel(g, GRAPH_LINE, dircolor,
		mc_get_double(meteoconfig, "wind.right.min", -400.),
		mc_get_double(meteoconfig, "wind.right.scale", 8.));

	/* retrieve the data from the database				*/
	if (debug)
		fprintf(stderr, "%s:%d: query is '%s'\n", __FILE__, __LINE__,
			query);
	if (mysql_query(dgp->mysql, query)) {
		fprintf(stderr, "%s:%d: cannot retrieve data\n", __FILE__,
			__LINE__);
		exit(EXIT_FAILURE);
	}
	res = mysql_store_result(dgp->mysql);

	/* find out how many rows there are				*/
	nentries = mysql_num_rows(res);
	if (debug)
		fprintf(stderr, "%s:%d: temp query returns %d rows\n",
			__FILE__, __LINE__, nentries);

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
			fprintf(stderr, "%s:%d: %9d %6.1f %6.1f %6.0f\n",
				__FILE__, __LINE__, (int)data[i].when,
				data[i].data[0], data[i].data[1],
				data[i].data[2]);
	}

	/* display the data retrieved from the database			*/
	graph_add_data(g, start, interval, nentries, data);

	/* display the grid						*/
	graph_add_time(g);
	graph_add_grid(g, 0,
		mc_get_double(meteoconfig, "wind.left.step", 5.),
		mc_get_double(meteoconfig, "wind.left.start", 0.),
		mc_get_double(meteoconfig, "wind.left.end", 10.));
	graph_add_grid(g, 2,
		mc_get_double(meteoconfig, "wind.right.step", 180.),
		mc_get_double(meteoconfig, "wind.right.start", 0.),
		mc_get_double(meteoconfig, "wind.right.end", 360.));
	graph_add_ticks(g,
		0,
		mc_get_double(meteoconfig, "wind.left.step", 5.),
		mc_get_double(meteoconfig, "wind.left.start", 0.),
		mc_get_double(meteoconfig, "wind.left.end", 10.),
		mc_get_string(meteoconfig, "wind.left.format", "%.0f"),
		0);
	graph_add_ticks(g,
		2,
		mc_get_double(meteoconfig, "wind.right.step", 180.),
		mc_get_double(meteoconfig, "wind.right.start", 0.),
		mc_get_double(meteoconfig, "wind.right.end", 360.),
		mc_get_string(meteoconfig, "wind.right.format", "%.0f"),
		1);

	/* compute the filename for the graph				*/
	snprintf(filename, sizeof(filename), "%s-wind-%s.png", dgp->prefix,
		dgp->suffix);
	graph_write_png(g, filename);
	if (debug)
		fprintf(stderr, "%s:%d: image written to %s\n", __FILE__,
			__LINE__, filename);
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
	graph_label(g, mc_get_string(meteoconfig, "radiation.left.label",
		"Solar Radiation (W/m2)"), 0);
	graph_label(g, mc_get_string(meteoconfig, "radiation.right.label",
		"UV index"), 1);

	solarcolor = graph_color_allocate(g, mc_get_color(meteoconfig,
		"radiation.solarcolor", radiation_default_solar_color));
	uvcolor = graph_color_allocate(g, mc_get_color(meteoconfig,
		"radiation.uvcolor", radiation_default_uv_color));
	graph_add_channel(g, GRAPH_HISTOGRAMM, solarcolor,
		mc_get_double(meteoconfig, "radiation.left.min", 0.),
		mc_get_double(meteoconfig, "radiation.left.scale", 5));
	graph_add_channel(g, GRAPH_LINE, uvcolor,
		mc_get_double(meteoconfig, "radiation.right.min", 0.),
		mc_get_double(meteoconfig, "radiation.right.scale", 0.05));

	/* retrieve the data from the database				*/
	if (debug)
		fprintf(stderr, "%s:%d: query is '%s'\n", __FILE__, __LINE__,
			query);
	if (mysql_query(dgp->mysql, query)) {
		fprintf(stderr, "%s:%d: cannot retrieve data\n", __FILE__,
			__LINE__);
		exit(EXIT_FAILURE);
	}
	res = mysql_store_result(dgp->mysql);

	/* find out how many rows there are				*/
	nentries = mysql_num_rows(res);
	if (debug)
		fprintf(stderr, "%s:%d: temp query returns %d rows\n",
			__FILE__, __LINE__, nentries);

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
			fprintf(stderr, "%s:%d: %9d %6.1f %6.1f\n",
				__FILE__, __LINE__, (int)data[i].when,
				data[i].data[0], data[i].data[1]);
	}

	/* display the data retrieved from the database			*/
	graph_add_data(g, start, interval, nentries, data);

	/* display the grid						*/
	graph_add_time(g);
	graph_add_grid(g, 0,
		mc_get_double(meteoconfig, "radiation.left.step", 100.),
		mc_get_double(meteoconfig, "radiation.left.start", 0.),
		mc_get_double(meteoconfig, "radiation.left.end", 600.));
	/* it's not necessary to write the grid lines twice...
	graph_add_grid(g, 1,
		mc_get_double(meteoconfig, "radiation.right.step", 1.),
		mc_get_double(meteoconfig, "radiation.right.start", 0.),
		mc_get_double(meteoconfig, "radiation.right.end", 6.));
	*/
	graph_add_ticks(g,
		0,
		mc_get_double(meteoconfig, "radiation.left.step", 100.),
		mc_get_double(meteoconfig, "radiation.left.start", 0.),
		mc_get_double(meteoconfig, "radiation.left.end", 600.),
		mc_get_string(meteoconfig, "radiation.left.format", "%.0f"),
		0);
	graph_add_ticks(g,
		1,
		mc_get_double(meteoconfig, "radiation.right.step", 1.),
		mc_get_double(meteoconfig, "radiation.right.start", 0.),
		mc_get_double(meteoconfig, "radiation.right.end", 6.),
		mc_get_string(meteoconfig, "radiation.right.format", "%.1f"),
		1);

	/* compute the filename for the graph				*/
	snprintf(filename, sizeof(filename), "%s-radiation-%s.png",
		dgp->prefix, dgp->suffix);
	graph_write_png(g, filename);
	if (debug)
		fprintf(stderr, "%s:%d: image written to %s\n", __FILE__,
			__LINE__, filename);
}

/*
 * all graphs
 */
void	all_graphs(dograph_t *dgp, int interval) {
	if (debug)
		fprintf(stderr, "%s:%d: creating all graphs for interval %d, "
			"ending at %d, suffix = '%s'\n", __FILE__, __LINE__,
			interval, (int)dgp->end, dgp->suffix);
	if (dgp->requestedgraphs & DOGRAPH_BAROMETER)
		baro_graphs(dgp, interval);
	if (dgp->requestedgraphs & DOGRAPH_TEMPERATURE)
		temp_graphs(dgp, interval);
	if (dgp->requestedgraphs & DOGRAPH_TEMPERATURE_INSIDE)
		temp_graphs_inside(dgp, interval);
	if (dgp->requestedgraphs & DOGRAPH_RAIN)
		rain_graphs(dgp, interval);
	if (dgp->requestedgraphs & DOGRAPH_WIND)
		wind_graphs(dgp, interval);
	if (dgp->requestedgraphs & DOGRAPH_RADIATION)
		radiation_graphs(dgp, interval);
}

