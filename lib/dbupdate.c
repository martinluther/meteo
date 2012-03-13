/*
 * dbupdate.c  -- perform an update to the database
 *
 * (c) 2001 Dr. Andreas Mueller
 *
 * $Id: dbupdate.c,v 1.2 2002/01/27 21:01:42 afm Exp $
 */
#include <dbupdate.h>
#include <database.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <msgque.h>
#include <mdebug.h>

dest_t	*dest_new(void) {
	dest_t	*result;
	result = (dest_t *)malloc(sizeof(dest_t));
	result->type = DEST_NONE;
	return result;
}

void	dest_free(dest_t *d) {
	switch (d->type) {
	case DEST_MYSQL:
		mc_closedb(d->destdata.mysql);
		break;
	case DEST_MSGQUE:
		break;
	default:
		break;
	}
	free(d);
}

static int	dbquery(dest_t *ddp, const char *query) {
	if (NULL == ddp)
		return -1;
	switch (ddp->type) {
	case DEST_NONE:
		mdebug(LOG_ERR, MDEBUG_LOG, 0,
			"dest not initialized for update");
		return -1;
		break;
	case DEST_MYSQL:
		if (debug)
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
				"sending query directly to database");
		return mysql_query(ddp->destdata.mysql, query);
		break;
	case DEST_MSGQUE:
		if (debug)
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
				"sending query to msg queue %d (length = %d)",
				ddp->destdata.msgque, strlen(query) + 1);
		return msgque_sendquery(ddp->destdata.msgque, query,
			strlen(query) + 1);
		break;
	}
	return -1;
}

/*
 * the dbupdate function takes the meteodata record and sends it to the
 * the mysql database.
 */
int	dbupdate(dest_t *ddp, meteodata_t *md, const char *station) {
	char		query[2048];
	char		timestamp[32];
	time_t		now;
	struct tm	*nt;
	double		humidity, humidity_inside;

	/* compute a time stamp						*/
	time(&now);
	now -= now % 60;
	nt = localtime(&now);
	strftime(timestamp, sizeof(timestamp), "%Y%m%d%H%M%S", nt);

	/* normalize the data						*/
	humidity = md->humidity->value;
	if (humidity > 100.)
		humidity = 100.;
	humidity_inside = md->humidity_inside->value;
	if (humidity_inside > 100.)
		humidity_inside = 100.;
	
	/* prepare the query						*/
        sprintf(query,
		"insert into meteo.stationdata(timekey, station, "
		"	year, month, mday, hour, min, "
		"	temperature, temperature_inside, barometer, "
		"	humidity, humidity_inside, rain, raintotal, "
		"	windspeed, winddir, windgust, windx, windy, "
		"	solar, uv, "
		"	duration, samples, "
		"	group300, group1800, group7200, group86400) "
		"values(%s, '%s', "
		"	%d, %d, %d, "
		"	%d, %d, "
		"	%.2f, %.2f, %.2f, %.0f, %.0f, %.1f, %.1f, "
		"	%.2f, %.0f, %.1f, %.4f, %.4f, "
		"	%.1f, %.1f, "
		"	%.6f, %d, "
		"	%ld, %ld, %ld, %ld)",
		timestamp, station,

		1900 + nt->tm_year, 1 + nt->tm_mon, nt->tm_mday,

		nt->tm_hour, nt->tm_min,

		unitconvert(md->temperature->unit, UNIT_CELSIUS,
			md->temperature->value),
		unitconvert(md->temperature_inside->unit, UNIT_CELSIUS,
			md->temperature_inside->value),
		unitconvert(md->barometer->unit, UNIT_HPA,
			md->barometer->value),
		humidity, humidity_inside,
		unitconvert(md->rain->unit, UNIT_MM, md->rain->rain),
		unitconvert(md->rain->unit, UNIT_MM, md->rain->raintotal),

		unitconvert(md->wind->unit, UNIT_MPS, md->wind->speed),
		md->wind->direction,
		unitconvert(md->wind->unit, UNIT_MPS, md->wind->speedmax),
		unitconvert(md->wind->unit, UNIT_MPS, md->wind->x),
		unitconvert(md->wind->unit, UNIT_MPS, md->wind->y),

		unitconvert(md->solar->unit, UNIT_WM2, md->solar->value),
		unitconvert(md->uv->unit, UNIT_UVINDEX, md->uv->value),

		(md->last.tv_sec - md->start.tv_sec)
			+ (md->last.tv_usec - md->start.tv_usec)/1000000.,
		md->samples,

		(now - 60)/300, (now - 60)/1800, (now - 60)/7200,
		(now - 60)/86400);
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "query is '%s'", query);
	if (dbquery(ddp, query)) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "update query failed");
		return -1;
	}
	return 0;
}
