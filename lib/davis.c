/*
 * davis.c -- get data from a davis weather station
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: davis.c,v 1.5 2003/05/04 16:31:58 afm Exp $
 */
#include <meteodata.h>
#include <davis.h>
#include <stdlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <meteo.h>
#include <mdebug.h>

static meteovalue_t	*davis_get_temperature(meteoaccess_t *m);
static meteovalue_t	*davis_get_temperature_inside(meteoaccess_t *m);
static meteovalue_t	*davis_get_humidity(meteoaccess_t *m);
static meteovalue_t	*davis_get_humidity_inside(meteoaccess_t *m);
static meteovalue_t	*davis_get_barometer(meteoaccess_t *m);
static int		davis_get_barotrend(meteoaccess_t *m);
#define	vantage_get_barotrend	davis_get_barotrend
static wind_t		*davis_get_wind(meteoaccess_t *m);
static rain_t		*davis_get_rain(meteoaccess_t *m);
static meteodata_t	*davis_get_data(meteoaccess_t *m);
static meteodata_t	*davis_get_update(meteoaccess_t *m);

#define	vantage_get_temperature	davis_get_temperature
#define	vantage_get_temperature_inside	davis_get_temperature_inside
#define	vantage_get_humidity davis_get_humidity
#define	vantage_get_humidity_inside davis_get_humidity_inside
#define	vantage_get_barometer davis_get_barometer
#define	vantage_get_wind davis_get_wind
#define	vantage_get_rain davis_get_rain
#define	vantage_get_data davis_get_data
#define vantage_get_update davis_get_update

static unsigned char	*get_station_nibbles(meteoaccess_t *m,
	unsigned int offset, unsigned int size) {
	unsigned char	*result;
	int		l;
	unsigned short	bank;

	bank = offset >> 8;
        if (debug)
                mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"%d nibbles from bank %d at offset %02x requested",
			size, bank, offset);

	put_string(m->m, "WRD");
	put_char(m->m, (size << 4) | (2 + (bank << 1)));
	put_char(m->m, offset);
	put_char(m->m, 0x0d);
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"command 'WRD %02x %02x' written",
			(size << 4) | (2 + (bank << 1)), offset);
	if (get_acknowledge(m->m) < 0) {
		return NULL;
	}
	l = (size + 1)/2;
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "expecting %d reply bytes", l);
	result = (unsigned char *)malloc(l);
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"(unsigned char *)malloc(%d) = %p",
			l, result);
	get_buffer(m->m, result, l);
	return result;
}

static unsigned char	*get_link_nibbles(meteoaccess_t *m, unsigned int offset,
	unsigned int size) {
	unsigned char	*result;
	int		l;
	unsigned short	bank;

	bank = offset >> 8;
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"%d nibbles from bank %d at offset %02x requested",
			size, bank, offset);
	put_string(m->m, "RRD");
	put_char(m->m, bank);
	put_char(m->m, offset);
	put_char(m->m, size - 1);
	put_char(m->m, 0x0d);
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"command 'RRD %d %02x %d' written",
			  bank, offset, size - 1);
	if (get_acknowledge(m->m) < 0) {
		return NULL;
	}
	l = (size + 1)/2;
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "expecting %d reply bytes", l);
	result = (unsigned char *)malloc(l);
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"(unsigned char *)malloc(%d) = %p",
			l, result);
	get_buffer(m->m, result, l);
	return result;
}

static int	bcd2int(unsigned char c) {
	int	i;
	i = 10 * (c >> 4) + (c & 0x0f);
	return i;
}

static time_t	get_station_time(meteoaccess_t *m,
	unsigned int timeoffset, unsigned int dateoffset) {
	unsigned char	*d, *t;
	struct tm	tm, *tmp;
	time_t		now;
	t = get_station_nibbles(m, timeoffset, 4);
	d = get_station_nibbles(m, dateoffset, 3);
	now = time(NULL);
	tmp = localtime(&now);
	tm.tm_sec = 0;
	tm.tm_min = bcd2int(t[1]);
	tm.tm_hour = bcd2int(t[0]);
	tm.tm_mday = bcd2int(d[0]);
	tm.tm_mon = d[1] - 1;
	tm.tm_year = tmp->tm_year;
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"free((unsigned char *)%p)", t);
	free(t);
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"free((unsigned char *)%p)", d);
	free(d);
	now = mktime(&tm);
	return now;
}

static double	get_station_value(meteoaccess_t *m, unsigned int offset,
	unsigned int size) {
	unsigned char	*t;
	int		l, i;
	unsigned short	bank;
	double		result;

	bank = offset >> 8;
	t = get_station_nibbles(m, offset, size);
	if (t == NULL)
		return 0.;
	l = (size + 1)/2;
	result = 0.;
	for (i = l - 1; i >= 0; i--) {
		result = result * 256. + t[i];
	}
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"free((unsigned char *)%p)", t);
	free(t);
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"value(offset = %02x, bank = %d, size = %d) = %10.4f",
			offset, bank, size, result);
	return result;
}

static double	get_link_value(meteoaccess_t *m, unsigned int offset,
	unsigned int size) {
	unsigned char	*t;
	int		l, i;
	unsigned short	bank;
	double		result;

	bank = offset >> 8;
	t = get_link_nibbles(m, offset, size);
	if (t == NULL)
		return 0.;
	l = (size + 1)/2;
	result = 0.;
	for (i = l - 1; i >= 0; i--) {
		result = result * 256. + t[i];
	}
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"free((unsigned char *)%p)", t);
	free(t);
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"value(offset = %02x, bank = %d, size = %d) = %10.4f",
			offset, bank, size, result);
	return result;
}

int	vantage_wakeup(meteocom_t *m) {
	struct timeval	tv;
	int		i, c;
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"this is Vantage station, try wake up");
	for (i = 0; i < 3; i++) {
		put_char(m, '\n');
		tv.tv_sec = 5; tv.tv_usec = 0;
		c = get_char_timed(m, &tv);
		switch (c) {
		case -1:
			mdebug(LOG_WARNING, MDEBUG_LOG, 0,
				"warning: waking up of Vantage station failed");
			break;
		case '\n':
			if (debug)
				mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
					"station: waked up");
			return 0;
			break;
		}
	}
	return -1;
}

static int	davis_sync(meteocom_t *m) {
	struct timeval	tv;
	int		c1, result = -1;

	/* sync'ing the communication with a davis station is easy	*/
	/* send a command that with an ACK reply with no data, wait	*/
	/* a few seconds (reading data all the time) and make sure the	*/
	/* last character you get is the ACK				*/
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "sync'ing Davis station");

	/* some Davis stations need to be awakened, most notably the	*/
	/* Vantage pro. So if the type string in the configuration is	*/
	/* Vantage, we must send a '\r' and wait for a '\n'		*/
	if (m->flags & COM_VANTAGE) {
		if (vantage_wakeup(m) < 0) {
			mdebug(LOG_ERR, MDEBUG_LOG, 0,
				"cannot wake up Vantage station");
			return -1;
		}
	}

	/* send start command, which usually does nothing		*/
	put_string(m, "START");
	put_char(m, 0x0d);

	/* start timer							*/
	tv.tv_sec = 5; tv.tv_usec = 0;
	c1 = get_acknowledge_timed(m, &tv);
	if (c1 >= 0) 
		result = 0;
	else
		mdebug(LOG_ERR, MDEBUG_LOG, 0,
			"return char is not an ACK, but 0x%02x", c1);

	/* we should now be sync'ed, write a debug message about the	*/
	/* state 							*/
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "station %ssync'ed", 
			 (result < 0) ? "not " : "");
	return result;
}

meteoaccess_t	*davis_new(meteocom_t *m) {
	meteoaccess_t	*md;
	unsigned char	*b;

	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"initializing Davis weather station");

	/* sync the communications with the station			*/
	if (davis_sync(m) < 0) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0,
			"synchronization with station failed");
		return NULL;
	}

	/* create a new structure					*/
	md = (meteoaccess_t *)malloc(sizeof(meteoaccess_t));
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"(meteoaccess_t *)malloc(%d) = %p",
			sizeof(meteoaccess_t), md);
	memset(md, 0, sizeof(meteoaccess_t));
	md->m = m;

	/* assign data communcation functions 				*/
	if (m->flags & COM_VANTAGE) {
		md->get_temperature = vantage_get_temperature;
		md->get_temperature_inside = vantage_get_temperature_inside;
		md->get_humidity = vantage_get_humidity;
		md->get_humidity_inside = vantage_get_humidity_inside;
		md->get_barometer = vantage_get_barometer;
		md->get_barotrend = vantage_get_barotrend;
		md->get_wind = vantage_get_wind;
		md->get_rain = vantage_get_rain;
		md->get_data = vantage_get_data;
		md->get_update = vantage_get_update;
	} else {
		md->get_temperature = davis_get_temperature;
		md->get_temperature_inside = davis_get_temperature_inside;
		md->get_humidity = davis_get_humidity;
		md->get_humidity_inside = davis_get_humidity_inside;
		md->get_barometer = davis_get_barometer;
		md->get_barotrend = davis_get_barotrend;
		md->get_wind = davis_get_wind;
		md->get_rain = davis_get_rain;
		md->get_data = davis_get_data;
		md->get_update = davis_get_update;
	}

	/* get the station type						*/
	if (!(m->flags & COM_VANTAGE)) {
		b = get_link_nibbles(md, 0x004d, 1);
		if (b == NULL) return NULL;
		md->private = malloc(sizeof(int));
		if (debug)
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
				"(int *)malloc(%d) = %p",
				sizeof(int), md->private);
		*(int *)md->private = (0x0f & (*b));
		if (debug)
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "version: %d",
				*(int *)md->private);
		if (debug)
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
				"free((unsigned char *)%p)", b);
		free(b);
	}

	/* return the initialized structure				*/
	return md;
}

static meteovalue_t	*davis_get_temperature(meteoaccess_t *m) {
	meteovalue_t	*v;

	/* allocate result memory					*/
	v = (meteovalue_t *)malloc(sizeof(meteovalue_t));
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"(meteovalue_t *)malloc(%d) = %p",
			sizeof(meteovalue_t), v);
	v->flags = METEOVALUE_TEMPERATURE;
	v->unit = UNIT_FAHRENHEIT;

	/* set result data						*/
	v->value = get_station_value(m, 0x0156, 4)/10.;
	if (v->value <= 1000)
		v->flags |= METEOVALUE_HASVALUE;

	/* try to get the high temperature				*/
	v->max = get_station_value(m, 0x015a, 4)/10.;
	v->maxtime = get_station_time(m, 0x0162, 0x016a);
	v->flags |= METEOVALUE_HASMAX;

	/* try to get lo temperature					*/
	v->min = get_station_value(m, 0x015e, 4)/10.;
	v->mintime = get_station_time(m, 0x0166, 0x016d);
	v->flags |= METEOVALUE_HASMIN;
	
	return v;
}

static meteovalue_t	*davis_get_temperature_inside(meteoaccess_t *m) {
	meteovalue_t	*v;

	/* allocate result memory					*/
	v = (meteovalue_t *)malloc(sizeof(meteovalue_t));
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"(meteovalue_t *)malloc(%d) = %p",
			sizeof(meteovalue_t), v);
	v->flags = METEOVALUE_TEMPERATURE_INSIDE;
	v->unit = UNIT_FAHRENHEIT;

	/* set result data						*/
	v->value = get_station_value(m, 0x0130, 4)/10.;
	if (v->value <= 1000)
		v->flags |= METEOVALUE_HASVALUE;

	/* try to get the high temperature				*/
	v->max = get_station_value(m, 0x0134, 4)/10.;
	v->maxtime = get_station_time(m, 0x013c, 0x0144);
	v->flags |= METEOVALUE_HASMAX;

	/* try to get lo temperature					*/
	v->min = get_station_value(m, 0x0138, 4)/10.;
	v->mintime = get_station_time(m, 0x0140, 0x0147);
	v->flags |= METEOVALUE_HASMIN;
	
	return v;
}

static meteovalue_t	*davis_get_humidity(meteoaccess_t *m) {
	meteovalue_t	*v;

	/* allocate memory						*/
	v = (meteovalue_t *)malloc(sizeof(meteovalue_t));
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"(meteovalue_t *)malloc(%d) = %p",
			sizeof(meteovalue_t), v);
	v->flags = METEOVALUE_HUMIDITY;
	v->unit = UNIT_NONE;

	/* set data							*/
	v->value = get_station_value(m, 0x0198, 2);
	if (v->value > 100)
		v->value = 100.;
	v->flags |= METEOVALUE_HASVALUE;

	/* max humidity							*/
	v->max = get_station_value(m, 0x019a, 2);
	v->maxtime = get_station_time(m, 0x019e, 0x01a6);
	v->flags |= METEOVALUE_HASMAX;

	/* min humidity							*/
	v->min = get_station_value(m, 0x019c, 2);
	v->mintime = get_station_time(m, 0x01a2, 0x01a9);
	v->flags |= METEOVALUE_HASMIN;
	
	return v;
}

static meteovalue_t	*davis_get_humidity_inside(meteoaccess_t *m) {
	meteovalue_t	*v;

	/* allocate memory						*/
	v = (meteovalue_t *)malloc(sizeof(meteovalue_t));
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"(meteovalue_t *)malloc(%d) = %p",
			sizeof(meteovalue_t), v);
	v->flags = METEOVALUE_HUMIDITY_INSIDE;
	v->unit = UNIT_NONE;

	/* set data							*/
	v->value = get_station_value(m, 0x0180, 2);
	if (v->value > 100)
		v->value = 100.;
	v->flags |= METEOVALUE_HASVALUE;

	/* max humidity							*/
	v->max = get_station_value(m, 0x0182, 2);
	v->maxtime = get_station_time(m, 0x0186, 0x018e);
	v->flags |= METEOVALUE_HASMAX;

	/* min humidity							*/
	v->min = get_station_value(m, 0x0184, 2);
	v->mintime = get_station_time(m, 0x018a, 0x0191);
	v->flags |= METEOVALUE_HASMIN;
	
	return v;
}

static meteovalue_t	*davis_get_barometer(meteoaccess_t *m) {
	meteovalue_t	*v;

	/* allocate memory						*/
	v = (meteovalue_t *)malloc(sizeof(meteovalue_t));
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"(meteovalue_t *)malloc(%d) = %p",
			sizeof(meteovalue_t), v);
	v->flags = METEOVALUE_BAROMETER;
	v->unit = UNIT_INHG;

	/* set result data						*/
	v->value = get_station_value(m, 0x0100, 4)/1000.;
	if (v->value < 1000)
		v->flags |= METEOVALUE_HASVALUE;

	return v;
}

static int	davis_get_barotrend(meteoaccess_t *m) {
	return BAROTREND_UNKNOWN;
}

static wind_t	*davis_get_wind(meteoaccess_t *m) {
	wind_t		*w;

	/* allocate memory						*/
	w = (wind_t *)malloc(sizeof(wind_t));
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"(wind_t *)malloc(%d) = %p",
			sizeof(wind_t), w);
	memset(w, 0, sizeof(w));

	/* get wind direction from station				*/
	w->speed = get_station_value(m, 0x005e, 2);
	w->unit = UNIT_MPH;
	if ((w->speed >= 0.0) && (w->speed <= 200.0))
		w->flags |= METEOVALUE_HASVALUE;

	w->direction = (int)get_station_value(m, 0x01b4, 4);
	if ((w->direction >= 0) && (w->direction <= 360)) {
		w->flags |= METEOVALUE_HASDIRECTION;
	}

	w->speedmax = get_station_value(m, 0x0060, 4);
	w->flags |= METEOVALUE_HASMAX;
	w->maxtime = get_station_time(m, 0x0064, 0x0068);

	return w;
}

static rain_t	*davis_get_rain(meteoaccess_t *m) {
	rain_t	*v;
	double	cal;

	v = (rain_t *)malloc(sizeof(rain_t));
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"(rain_t *)malloc(%d) = %p",
			  sizeof(rain_t), v);
	v->rain = get_station_value(m, 0x01d2, 4);
	v->raintotal = get_station_value(m, 0x01ce, 4);
	cal = get_station_value(m, 0x01d6, 4);
	/* XXX there seems to be a bug in the weather station, 		*/
	/* resulting in an incorrect cal number being returned		*/
	cal = 5.;
	v->rain /= cal;
	v->raintotal /= cal;
	if (cal < 6.)
		v->unit = UNIT_MM;
	else
		v->unit = UNIT_IN;
	v->flags = METEOVALUE_HASTOTAL | METEOVALUE_HASVALUE;
	return v;
}

static meteodata_t	*davis_get_data(meteoaccess_t *m) {
	return NULL;
}

/*
 * the get_update function uses the archive function of the weather link
 * to retrieve info about the last measurement interval. It first forces
 * an archive update, and then reads the data from the archive image.
 * This allows to read some data that would otherwise only be available
 * from station, like maximum values etc.
 */
static meteodata_t	*davis_get_update(meteoaccess_t *m) {
	meteodata_t	*results;

	/* allocate a result structure					*/
	results = (meteodata_t *)malloc(sizeof(meteodata_t));
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"(meteodata_t *)malloc(%d) = %p",
			sizeof(meteodata_t), results);
	memset(results, 0, sizeof(meteodata_t));

	/* disable the archive timer					*/
	put_string(m->m, "EBT");
	put_char(m->m, 0x0d);
	if (get_acknowledge(m->m) < 0) {
		return NULL;
	}

	/* force a write to the archive image				*/
	put_string(m->m, "ARC");
	put_char(m->m, 0x0d);
	if (get_acknowledge(m->m) < 0) {
		return NULL;
	}

	/* create structures for result data				*/
	results->temperature
		= (meteovalue_t *)malloc(sizeof(meteovalue_t));
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"(meteovalue_t *)malloc(%d) = %p",
			sizeof(meteovalue_t), results->temperature);
	memset(results->temperature, 0, sizeof(meteovalue_t));

	results->temperature_inside
		= (meteovalue_t *)malloc(sizeof(meteovalue_t));
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"(meteovalue_t *)malloc(%d) = %p",
			sizeof(meteovalue_t), results->temperature_inside);
	memset(results->temperature_inside, 0, sizeof(meteovalue_t));

	results->humidity
		= (meteovalue_t *)malloc(sizeof(meteovalue_t));
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"(meteovalue_t *)malloc(%d) = %p",
			sizeof(meteovalue_t), results->humidity);
	memset(results->humidity, 0, sizeof(meteovalue_t));

	results->humidity_inside
		= (meteovalue_t *)malloc(sizeof(meteovalue_t));
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"(meteovalue_t *)malloc(%d) = %p",
			sizeof(meteovalue_t), results->humidity_inside);
	memset(results->humidity_inside, 0, sizeof(meteovalue_t));

	results->barometer
		= (meteovalue_t *)malloc(sizeof(meteovalue_t));
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"(meteovalue_t *)malloc(%d) = %p",
			sizeof(meteovalue_t), results->barometer);
	memset(results->barometer, 0, sizeof(meteovalue_t));

	results->wind = (wind_t *)malloc(sizeof(wind_t));
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"(wind_t *)malloc(%d) = %p",
			sizeof(wind_t), results->wind);
	memset(results->wind, 0, sizeof(wind_t));

	results->rain = (rain_t *)malloc(sizeof(rain_t));
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "(rain_t *)malloc(%d) = %p",
			sizeof(rain_t), results->rain);
	memset(results->rain, 0, sizeof(rain_t));
	
	/* read values from archive image				*/
	/* barometer							*/
	results->barometer->value = get_link_value(m, 0x188, 4)/1000.;
	results->barometer->unit = UNIT_INHG;
	results->barometer->flags
		|= METEOVALUE_HASVALUE | METEOVALUE_BAROMETER;
	results->barotrend = BAROTREND_UNKNOWN;

	/* outside humidity						*/
	results->humidity->value = get_link_value(m, 0x18e, 2);
	results->humidity->unit = UNIT_NONE;
	results->humidity->flags
		|= METEOVALUE_HASVALUE | METEOVALUE_HUMIDITY;

	/* inside humidity						*/
	results->humidity_inside->value = get_link_value(m, 0x18c, 2);
	results->humidity_inside->unit = UNIT_NONE;
	results->humidity_inside->flags
		|= METEOVALUE_HASVALUE | METEOVALUE_HUMIDITY_INSIDE;

	/* rain in period						*/
	results->rain->rain = get_link_value(m, 0x190, 4);
	results->rain->unit = UNIT_NONE;
	results->rain->flags
		|= METEOVALUE_HASVALUE;

	/* temperature inside						*/
	results->temperature_inside->value = get_link_value(m, 0x194, 4)/10.;
	results->temperature_inside->unit = UNIT_FAHRENHEIT;
	results->temperature_inside->flags
		|= METEOVALUE_HASVALUE | METEOVALUE_TEMPERATURE_INSIDE;

	/* temperature outside						*/
	results->temperature->value = get_link_value(m, 0x198, 4)/10.;
	results->temperature->unit = UNIT_FAHRENHEIT;
	results->temperature->flags
		|= METEOVALUE_HASVALUE | METEOVALUE_TEMPERATURE;

	/* wind 							*/
	results->wind->speed = get_link_value(m, 0x19c, 2);
	results->wind->speedmax = get_link_value(m, 0x1a4, 2);
	results->wind->direction = get_link_value(m, 0x19e, 2);
	results->wind->flags |= METEOVALUE_HASVALUE | METEOVALUE_HASMAX;
	results->wind->unit = UNIT_MPH;
	
	return results;
}
