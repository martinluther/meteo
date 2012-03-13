/*
 * average.c -- compute averages for the various parameters measured by
 *              the station
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: average.c,v 1.9 2003/07/19 12:44:33 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <average.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#include <meteo.h>
#include <math.h>
#include <mdebug.h>

#define	TRUE	1
#define	FALSE	0

int	average_fake = 0;

typedef struct querydata_s {
	MYSQL	*mysql;
	time_t	from, to;
	int	interval, group;
	char	*station;
} querydata_t;

typedef struct avgdata_s {
	double	values[21];	int	isvalid[21];
#define	X_TEMPERATURE	 		0
#define	X_TEMPERATURE_MAX 		1
#define	X_TEMPERATURE_MIN 		2
#define	X_TEMPERATURE_INSIDE 		3
#define	X_TEMPERATURE_INSIDE_MAX	4
#define	X_TEMPERATURE_INSIDE_MIN	5
#define	X_BAROMETER			6
#define X_BAROMETER_MAX			7
#define X_BAROMETER_MIN			8
#define X_HUMIDITY			9
#define X_HUMIDITY_MAX			10
#define X_HUMIDITY_MIN			11
#define	X_HUMIDITY_INSIDE		12
#define	X_HUMIDITY_INSIDE_MAX		13
#define	X_HUMIDITY_INSIDE_MIN		14
#define X_RAIN				15
#define X_WINDSPEED			16
#define X_WINDDIR			17
#define X_WINDGUST			18
#define X_SOLAR				19
#define X_UV				20
} avgdata_t;

char	*fieldnames[21] = {
	"temperature",
	"temperature_max",
	"temperature_min",
	"temperature_inside",
	"temperature_inside_max",
	"temperature_inside_min",
	"barometer",
	"barometer_max",
	"barometer_min",
	"humidity",
	"humidity_max",
	"humidity_min",
	"humidity_inside",
	"humidity_inside_max",
	"humidity_inside_min",
	"rain",
	"windspeed",
	"winddir",
	"windgust",
	"solar",
	"uv"
};

static MYSQL_RES	*perform_query(querydata_t *qdp) {
	char		query[2048];
	MYSQL_RES	*res;
	int		nrows, i;

	mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
		"perform_query from %d to %d, station %s",
		qdp->from, qdp->to, qdp->station);
	
	/* compute the query string					*/
	strcpy(query, "select ");
	for (i = 0; i < 15; i++) {
		switch (i % 3) {
		case 0:	strcat(query, "avg("); break;
		case 1:	strcat(query, "max("); break;
		case 2:	strcat(query, "min("); break;
		}
		strcat(query, fieldnames[i - (i%3)]);
		strcat(query, "), ");
	}
	strcat(query, "sum(rain), ");

	/* handle wind [16-19] and solar/uv [20, 21]			*/
	strcat(query, "sum(windx * duration), sum(windy * duration), ");
	strcat(query, "sum(duration), max(windgust), ");
	strcat(query, "avg(solar), avg(uv), count(*) ");
	snprintf(query + strlen(query), sizeof(query) - strlen(query),
		"from stationdata "
		"where timekey between %d and %d "
		"  and group%d = %d and station = '%-8.8s'",
		(int)qdp->from, (int)qdp->to, qdp->interval,
		qdp->group, qdp->station);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "averageing query is '%s'", query);

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
			"%d rows returned instead of one", nrows);
		return NULL;
	}

	/* return the result set					*/
	return res;
}

static int	perform_update(querydata_t *qdp, avgdata_t *adp) {
	char	query[2048];
	int	i;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "perparing update query");

	/* construct the insert query					*/
	snprintf(query, sizeof(query),
		"insert into averages (timekey, station, intval, ");
	for (i = 0; i < 21; i++) {
		strcat(query, fieldnames[i]);
		if (i < 20)
			strcat(query, ", ");
	}
	strcat(query, ") values (");
	snprintf(query + strlen(query), sizeof(query) - strlen(query),
		"%d, '%-8.8s', %d, ",
		(int)qdp->to, qdp->station, qdp->interval);

	/* add the values						*/
	for (i = 0; i < 21; i++) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "adding field %s",
			fieldnames[i]);
		/* regular values					*/
		if (adp->isvalid[i]) {
			snprintf(query + strlen(query),
				sizeof(query) - strlen(query),
				"/* %s */ %.1f, ", fieldnames[i],
					adp->values[i]);
		} else {
			snprintf(query + strlen(query),
				sizeof(query) - strlen(query),
				"/* %s */ NULL, ", fieldnames[i]);
		}
	}
	strcpy(query + strlen(query) - 2, ")");

	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "insert query for averages: '%s'",
		query);
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
	int		rows = 0, i;

	/* compute the timestamps					*/
	qd.to = now - (now % interval);
	qd.from = qd.to - interval;
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
	res = perform_query(&qd);
	if (res == NULL) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0,
			"nothing returned from avg query");
		return -1;
	} else
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "query returns row");

	/* get the data row						*/
	row = mysql_fetch_row(res);

	/* retrieve the values from the returned row and compute avgs	*/
	for (i = 0; i <= X_RAIN; i++) {
		/* regular values, including rain 			*/
		if (row[i]) {
			avg.values[i] = atof(row[i]);
			avg.isvalid[i] = TRUE;
		} else {
			avg.isvalid[i] = FALSE;
		}
	}
	/* wind speed and direction					*/
#define	WIND_X	16
#define	WIND_Y	17
#define	WIND_DUR 18
#define	WIND_MAX 19
	if (row[WIND_X]) {
		x = (row[WIND_X]) ? atof(row[WIND_X]) : 0.;
		y = (row[WIND_Y]) ? atof(row[WIND_Y]) : 0.;
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "wind vector: (%.1f, %.1f)",
			x, y);
		avg.values[X_WINDSPEED] = sqrt(x*x + y*y)/atof(row[WIND_DUR]);
		avg.values[X_WINDDIR] = 180. + 180. * atan2(x, y)/3.141592653;
		avg.isvalid[X_WINDSPEED] = TRUE;
		avg.isvalid[X_WINDDIR] = TRUE;
	} else {
		avg.values[X_WINDSPEED] = 0.; avg.isvalid[X_WINDSPEED] = FALSE;
		avg.values[X_WINDDIR] = 0.; avg.isvalid[X_WINDDIR] = FALSE;
	}
	/* max wind speed, solar and uv					*/
	for (i = WIND_MAX; i < 22; i++) {
		if (row[i]) {
			avg.values[i - 1] = atof(row[i]);
			avg.isvalid[i - 1] = TRUE;
		} else {
			avg.isvalid[i - 1] = FALSE;
		}
	}
	rows = (row[22]) ? atoi(row[22]) : 0;

	/* free the result pointer					*/
	mysql_free_result(res);

	/* create an update query to insert the data into the averages	*/
	/* table							*/
	if (rows > 0) {
		return perform_update(&qd, &avg);
	} else {
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
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "have_average-query is '%s'",
		query);


	/* perform query						*/
	if (mysql_query(mysql, query)) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "have_average-query "
			"failed, assume average does not exit");
		return 0;
	}

	/* retrieve result data						*/
	res = mysql_store_result(mysql);
	if (mysql_num_rows(res) != 1) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "count(*) does not "
			"return a row, this should not happen");
		mysql_free_result(res);
		return 0;
	}

	/* retrieve the counter						*/
	row = mysql_fetch_row(res);
	if (row[0]) {
		if (0 < atoi(row[0])) {
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
