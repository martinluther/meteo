/*
 * dbupdate.c  -- perform an update to the database
 *
 * (c) 2001 Dr. Andreas Mueller
 *
 * $Id: dbupdate.c,v 1.6 2003/05/04 16:31:58 afm Exp $
 */
#include <dbupdate.h>
#include <database.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <msgque.h>
#include <mdebug.h>
#include <unistd.h>

dest_t	*dest_new(void) {
	dest_t	*result;
	result = (dest_t *)malloc(sizeof(dest_t));
	result->type = DEST_NONE;
	result->name = NULL;
	return result;
}

void	dest_free(dest_t *d) {
	if (d->name != NULL) {
		free(d->name); d->name = NULL;
	}
	switch (d->type) {
	case DEST_MYSQL:
		mc_closedb(d->destdata.mysql);
		break;
	case DEST_MSGQUE:
		break;
	case DEST_FILE:
		close(d->destdata.file);
		break;
	default:
		break;
	}
	free(d);
}

static int	dbquery(dest_t *ddp, const char *query) {
	int	rc;
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
		rc = mysql_query(ddp->destdata.mysql, query);
		if (rc) {
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
				"update query failed: %s", ddp->destdata.mysql);
		}
		return rc;
		break;
	case DEST_MSGQUE:
		if (debug)
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
				"sending query to msg queue %s/%d "
				"(length = %d)", ddp->name,
				ddp->destdata.msgque, strlen(query) + 1);
		return msgque_sendquery(ddp->destdata.msgque, query,
			strlen(query) + 1);
		break;
	case DEST_FILE:
		if (debug)
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
				"sending query to file %s/%d "
				"(length = %d)", ddp->name,
				ddp->destdata.file, strlen(query) + 1);
		write(ddp->destdata.file, query, strlen(query));
		write(ddp->destdata.file, ";\n", 2);
		break;
	}
	return -1;
}

#define	value_string(outbuffer, isvalid, fmtstring, value)		\
	(outbuffer = (char *)alloca(32),				\
	((isvalid) ? sprintf(outbuffer, fmtstring, value)		\
	: sprintf(outbuffer, "NULL")), outbuffer)

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
	char		*temperature_string, *temperature_inside_string,
			*barometer_string,
			*humidity_string, *humidity_inside_string,
			*rain_string, *raintotal_string,
			*windspeed_string, *windgust_string,
			*windx_string, *windy_string, *solar_string, *uv_string;

	/* compute a time stamp						*/
	time(&now);
	now -= now % 60;
	nt = gmtime(&now);
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
		"insert into stationdata(timekey, station, "
		"	year, month, mday, hour, min, "
		"	temperature, temperature_inside, barometer, "
		"	humidity, humidity_inside, rain, raintotal, "
		"	windspeed, winddir, windgust, windx, windy, "
		"	solar, uv, "
		"	duration, samples, "
		"	group300, group1800, group7200, group86400) "
		"values(%d, '%s', "
		"	%d, %d, %d, "
		"	%d, %d, "
		"	%s, %s, %s, %s, %s, %s, %s, "
		"	%s, %.0f, %s, %s, %s, "
		"	%s, %s, "
		"	%.6f, %d, "
		"	%ld, %ld, %ld, %ld)",
		(int)now, station,

		1900 + nt->tm_year, 1 + nt->tm_mon, nt->tm_mday,

		nt->tm_hour, nt->tm_min,

		value_string(temperature_string, md->temperature->valid, "%.2f",
			unitconvert(md->temperature->unit, UNIT_CELSIUS,
				md->temperature->value)),
		value_string(temperature_inside_string,
			md->temperature_inside->valid, "%.2f",
			unitconvert(md->temperature_inside->unit, UNIT_CELSIUS,
				md->temperature_inside->value)),
		value_string(barometer_string, md->barometer->valid, "%.2f",
			unitconvert(md->barometer->unit, UNIT_HPA,
				md->barometer->value)),
		value_string(humidity_string, md->humidity->valid, "%.0f",
			humidity),
		value_string(humidity_inside_string, md->humidity_inside->valid,
			"%.0f", humidity),
		value_string(rain_string, md->rain->valid, "%.1f",
			unitconvert(md->rain->unit, UNIT_MM, md->rain->rain)),
		value_string(raintotal_string, md->rain->valid, "%.1f",
			unitconvert(md->rain->unit, UNIT_MM,
				md->rain->raintotal)),

		value_string(windspeed_string, md->wind->valid, "%.2f",
			unitconvert(md->wind->unit, UNIT_MPS, md->wind->speed)),
		md->wind->direction,
		value_string(windgust_string, md->wind->valid, "%.1f",
			unitconvert(md->wind->unit, UNIT_MPS,
			md->wind->speedmax)),
		value_string(windx_string, md->wind->valid, "%.4f",
			unitconvert(md->wind->unit, UNIT_MPS, md->wind->x)),
		value_string(windy_string, md->wind->valid, "%.4f",
			unitconvert(md->wind->unit, UNIT_MPS, md->wind->y)),

		value_string(solar_string, md->solar->valid, "%.0f",
			unitconvert(md->solar->unit, UNIT_WM2,
				md->solar->value)),
		value_string(uv_string, md->uv->valid, "%.0f",
			unitconvert(md->uv->unit, UNIT_UVINDEX, md->uv->value)),

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
