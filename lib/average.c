/*
 * average.c -- compute averages for the various parameters measured by
 *              the station
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: average.c,v 1.4 2002/11/18 02:36:42 afm Exp $
 */
#include <average.h>
#include <stdlib.h>
#include <stdio.h>
#include <meteo.h>
#include <math.h>
#include <mdebug.h>

int	average_fake = 0;

typedef struct querydata_s {
	MYSQL	*mysql;
	time_t	from, to;
	int	interval, group;
	char	*station;
} querydata_t;

typedef struct avgdata_s {
	double	temperature;
	double	temperature_max;
	double	temperature_min;
	double	temperature_inside;
	double	temperature_inside_max;
	double	temperature_inside_min;
	double	barometer;
	double	barometer_max;
	double	barometer_min;
	double	humidity;
	double	humidity_max;
	double	humidity_min;
	double	humidity_inside;
	double	humidity_inside_max;
	double	humidity_inside_min;
	double	rain;
	double	windspeed;
	double	winddir;
	double	windgust;
	double	solar;
	double	uv;
} avgdata_t;

static MYSQL_RES	*perform_query(querydata_t *qdp, const char *clause) {
	char		query[2048];
	MYSQL_RES	*res;
	int		nrows;

	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"perform_query from %d to %d, station %s\n",
			qdp->from, qdp->to, qdp->station);
	
	/* compute the query string					*/
	snprintf(query, sizeof(query),
		"select %s "
		"from stationdata "
		"where timekey between %d and %d "
		"  and group%d = %d and station = '%-8.8s'", clause,
		(int)qdp->from, (int)qdp->to, qdp->interval,
		qdp->group, qdp->station);
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "averageing query is '%s'",
			query);

	/* perform the query						*/
	if (mysql_query(qdp->mysql, query)) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "query for averages failed: %s",
			mysql_error(qdp->mysql));
		return NULL;
	}

	/* make sure the right number of rows is returned		*/
	res = mysql_store_result(qdp->mysql);
	if ((nrows = mysql_num_rows(res)) != 1) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0,
			"%d rows returned instead of one\n", nrows);
		return NULL;
	}

	/* return the result set					*/
	return res;
}

static int	perform_update(querydata_t *qdp, avgdata_t *adp) {
	char	query[2048];
	/* construct the insert query					*/
	snprintf(query, sizeof(query),
		"insert into averages ("
		"	timekey, station, intval, "
		"	temperature, temperature_max, temperature_min, "
		"	temperature_inside, temperature_inside_max, "
		"		temperature_inside_min, "
		"	barometer, barometer_max, barometer_min, "
		"	humidity, humidity_max, humidity_min, "
		"	humidity_inside, humidity_inside_max, "
		"		humidity_inside_min, "
		"	rain, "
		"	windspeed, winddir, windgust, "
		"	solar, uv ) "
		"values ("
		"	%d, '%-8.8s', %d, "
		"	%.1f, %.1f, %.1f, "
		"	%.1f, %.1f, %.1f, "
		"	%.1f, %.1f, %.1f, "
		"	%.0f, %.0f, %.0f, "
		"	%.0f, %.0f, %.0f, "
		"	%.1f, "
		"	%.1f, %.0f, %.1f, "
		"	%.1f, %.2f "
		")",
		(int)qdp->to, qdp->station, qdp->interval,
		adp->temperature, adp->temperature_max, adp->temperature_min,
		adp->temperature_inside, adp->temperature_inside_max,
			adp->temperature_inside_min,
		adp->barometer, adp->barometer_max, adp->barometer_min,
		adp->humidity, adp->humidity_max, adp->humidity_min,
		adp->humidity_inside, adp->humidity_inside_max,
			adp->humidity_inside_min,
		adp->rain,
		adp->windspeed, adp->winddir, adp->windgust,
		adp->solar, adp->uv);
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"insert query for averages: '%s'", query);
	if (average_fake) {
		mdebug(LOG_INFO, MDEBUG_LOG, 0, "insert faked");
		return 0;
	}

	/* perform the update						*/
	if (mysql_query(qdp->mysql, query)) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "insert query failed: %s",
			mysql_error(qdp->mysql));
		return -1;
	}
	return 0;
}

int	add_average(MYSQL *mysql, const time_t now, const int interval,
	const char *station) {
	querydata_t	qd;
	MYSQL_RES	*res;
	MYSQL_ROW	row;
	avgdata_t	avg;
	double		x, y;
	char		query[1024];
	int		rows = 0;

	/* compute the timestamps					*/
	qd.to = now - (now % interval);
	qd.from = qd.to - interval;
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"average for points between %d and %d", qd.from, qd.to);

	/* compute the group number					*/
	qd.group = qd.from / interval;
	qd.interval = interval;

	/* remaining parameters						*/
	qd.mysql = mysql;
	qd.station = (char *)station;

	/* delete the row if it happens to exists			*/
	snprintf(query, sizeof(query),
		"delete from averages "
		"where timekey = %d and intval = %d and station = '%-8.8s'",
		(int)qd.to, interval, station);
	if (mysql_query(mysql, query)) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "cannot delete row: %s",
			mysql_error(mysql));
	}

	/*
	 * perform a query for all the values we need to compute averages
	 */
	res = perform_query(&qd,
		"avg(barometer), max(barometer), min(barometer), "
		"avg(temperature), max(temperature), min(temperature), "
		"avg(temperature_inside), max(temperature_inside), "
		"	min(temperature_inside), "
		"avg(humidity), max(humidity), min(humidity), "
		"avg(humidity_inside), max(humidity_inside), "
		"	min(humidity_inside), "
		"sum(rain), "
		"sum(windx), sum(windy), sum(duration), max(windgust), "
		"avg(solar), avg(uv), count(*) ");
	if (res == NULL) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0,
			"nothing returned from avg query");
		return -1;
	} else
		if (debug)
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "query returns row");

	/* get the data row						*/
	row = mysql_fetch_row(res);

	/* retrieve the values from the returned row and compute avgs	*/
	avg.barometer = (row[0]) ? atof(row[0]) : 0.;
	avg.barometer_max = (row[1]) ? atof(row[1]) : 0.;
	avg.barometer_min = (row[2]) ? atof(row[2]) : 0.;
	avg.temperature = (row[3]) ? atof(row[3]) : 0.;
	avg.temperature_max = (row[4]) ? atof(row[4]) : 0.;
	avg.temperature_min = (row[5]) ? atof(row[5]) : 0.;
	avg.temperature_inside = (row[6]) ? atof(row[6]) : 0.;
	avg.temperature_inside_max = (row[7]) ? atof(row[7]) : 0.;
	avg.temperature_inside_min = (row[8]) ? atof(row[8]) : 0.;
	avg.humidity = (row[9]) ? atof(row[9]) : 0.;
	avg.humidity_max = (row[10]) ? atof(row[10]) : 0.;
	avg.humidity_min = (row[11]) ? atof(row[11]) : 0.;
	avg.humidity_inside = (row[12]) ? atof(row[12]) : 0.;
	avg.humidity_inside_max = (row[13]) ? atof(row[13]) : 0.;
	avg.humidity_inside_min = (row[14]) ? atof(row[14]) : 0.;
	avg.rain = (row[15]) ? atof(row[15]) : 0.;
	if (row[18]) {
		x = (row[16]) ? atof(row[16]) : 0.;
		y = (row[17]) ? atof(row[17]) : 0.;
		avg.windspeed = sqrt(x*x + y*y)/atof(row[18]);
		avg.winddir = 180. + 180. * atan2(x, y)/3.141592653;
	} else {
		avg.windspeed = 0.;
		avg.winddir = 0.;
	}
	avg.windgust = (row[19]) ? atof(row[19]) : 0.;
	avg.solar = (row[20]) ? atof(row[20]) : 0.;
	avg.uv = (row[21]) ? atof(row[21]) : 0.;
	rows = (row[22]) ? atoi(row[22]) : 0;

	/* free the result pointer					*/
	mysql_free_result(res);

	/* create an update query to insert the data into the averages	*/
	/* table							*/
	if (rows > 0) {
		return perform_update(&qd, &avg);
	} else {
		if (debug)
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
				"no rows for averages table found");
		return 0;
	}
}

int	have_average(MYSQL *mysql, const time_t now, const int interval,
		const char *station) {
	char		query[1024];
	MYSQL_RES	*res;
	MYSQL_ROW	row;

	/* make sure the interval matches the time			*/
	if (0 != (now % interval)) {
		if (debug)
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "point in time does "
				"not match interval %d %% %d = %d",
				(int)now, interval, ((int)now) % interval);
		return 1;
	}

	/* query averages table for a row				*/
	snprintf(query, sizeof(query),
			"select count(*) "
			"from averages "
			"where	timekey = %d "
			"	and station = '%-8.8s' "
			"	and intval = %d", (int)now, station, interval);
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "have_average-query is '%s'",
			query);


	/* perform query						*/
	if (mysql_query(mysql, query)) {
		if (debug)
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "have_average-query "
				"failed, assume average does not exit");
		return 0;
	}

	/* retrieve result data						*/
	res = mysql_store_result(mysql);
	if (mysql_num_rows(res) != 1) {
		if (debug)
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "count(*) does not "
				"return a row, this should not happen");
		mysql_free_result(res);
		return 0;
	}

	/* retrieve the counter						*/
	row = mysql_fetch_row(res);
	if (row[0]) {
		if (0 < atoi(row[0])) {
			if (debug)
				mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
					"average %d/%d positively exists",
					now, interval);
			mysql_free_result(res);
			return 1;
		}
	}

	/* cleanup							*/
	mysql_free_result(res);
	return 0;
}
