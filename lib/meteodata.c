/*
 * meteodata.c -- abstract meteo data implementation
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: meteodata.c,v 1.1 2002/01/18 23:34:29 afm Exp $
 */
#include <meteodata.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <meteo.h>

static double		lastinterval, nowinterval;

static double	weighted_average(double valuenow, double valuelast) {
	return (nowinterval * valuenow + lastinterval * valuelast) /
		(nowinterval + lastinterval);
}

void	meteovalue_set(meteovalue_t *m, double value) {
	m->value = weighted_average(value, m->value);
	if (value < m->min) {
		m->min = value;
		time(&m->mintime);
	}
	if (value > m->max) {
		m->max = value;
		time(&m->maxtime);
	}
}

#define	PI	3.1415926535
void	meteowind_set(wind_t *m, double speed /* MPS */, double direction) {
	int	dirindex;
	if (debug)
		fprintf(stderr, "%s:%d: meteowind_set(speed = %.1f, "
			"direction = %.1f\n", __FILE__,
			__LINE__, speed, direction);
	if (speed > m->speedmax) {
		m->speedmax = speed;
		time(&m->maxtime);
	}
	m->x += speed * nowinterval * sin(PI * direction/180.);
	m->y += speed * nowinterval * cos(PI * direction/180.);
	if (debug)
		fprintf(stderr, "%s:%d: Dx = %.1f, Dy = %.1f, Dt = %.1f\n",
			__FILE__, __LINE__, m->x, m->y,
			nowinterval + lastinterval);
	m->speed = sqrt((m->x * m->x) + (m->y * m->y)) /
		(nowinterval + lastinterval);
	m->direction = 180. * atan2(m->x, m->y)/PI;
	if (m->direction < 0) m->direction += 360.;
	dirindex = (int)(direction/22.5);
	if (dirindex < 0) dirindex = 0;
	if (dirindex >= 16) dirindex = 0;
	m->directions[dirindex]++;
}

void	meteorain_set(rain_t *r, double value) {
	if (r->raintotal != value)
		r->rain += (value - r->raintotal);
	r->raintotal = value;
}

meteodata_t	*meteodata_new(void) {
	meteodata_t	*result;
	result = (meteodata_t *)malloc(sizeof(meteodata_t));
	if (debug)
		fprintf(stderr, "%s:%d: (meteodata_t *)malloc(%d) = %p\n",
			__FILE__, __LINE__, sizeof(meteodata_t), result);
	memset(result, 0, sizeof(meteodata_t));
	return result;
}

void	meteodata_free(meteodata_t *a) {
	if (debug)
		fprintf(stderr, "%s:%d: free((meteodata_t *)%p)\n",
			__FILE__, __LINE__, a);
}

void	meteodata_update(meteodata_t *a,
	double temperature, double temperature_inside, /* FAHRENHEIT */
	double humidity, double humidity_inside, /* % HUMIDITY */
	double barometer, /* IN HG */
	double speed /* MPH */, double direction /* DEGREE */,
	double rain /* MM */, double solar /* WM2 */, double uv /* INDEX */) {
	struct timeval	now;

	/* get the current time to compute the time interval since the	*/
	/* last update 							*/
	gettimeofday(&now, NULL);
	nowinterval = (now.tv_sec - a->last.tv_sec)
		+ (now.tv_usec - a->last.tv_usec)/1000000.;
	lastinterval = (a->last.tv_sec - a->start.tv_sec)
		+ (a->last.tv_usec - a->start.tv_usec)/1000000.;

	/* update the rain total					*/
	if (a->rain->raintotal < 0)
		a->rain->raintotal = rain;

	/* update all the values in the meteodata_t structure		*/
	meteovalue_set(a->temperature, temperature);
	meteovalue_set(a->temperature_inside, temperature_inside);
	meteovalue_set(a->humidity, humidity);
	meteovalue_set(a->humidity_inside, humidity_inside);
	meteovalue_set(a->barometer, barometer);
	meteowind_set(a->wind, speed, direction);
	meteorain_set(a->rain, rain);
	meteovalue_set(a->solar, solar);
	meteovalue_set(a->uv, uv);

	/* update sample count and time of last update			*/
	a->samples++;
	memcpy(&a->last, &now, sizeof(now));
}

void	meteodata_start(meteodata_t *a) {
	gettimeofday(&a->start, NULL);
	gettimeofday(&a->last, NULL);
	a->samples = 0;

	if (a->temperature == NULL) {
		a->temperature = (meteovalue_t *)malloc(sizeof(meteovalue_t));
		if (debug)
			fprintf(stderr, "%s:%d: (meteovalue_t *)malloc(%d)"
				" = %p\n", __FILE__, __LINE__,
				sizeof(meteovalue_t), a->temperature);
	}
	a->temperature->value = 0.;
	a->temperature->max = -10000.;
	a->temperature->min = 10000.;
	a->temperature->unit = UNIT_FAHRENHEIT;

	if (a->temperature_inside == NULL) {
		a->temperature_inside = (meteovalue_t *)malloc(
			sizeof(meteovalue_t));
		if (debug)
			fprintf(stderr, "%s:%d: (meteovalue_t *)malloc(%d)"
				" = %p\n", __FILE__, __LINE__,
				sizeof(meteovalue_t), a->temperature_inside);
	}
	a->temperature_inside->value = 0.;
	a->temperature_inside->max = -10000.;
	a->temperature_inside->min = 10000.;
	a->temperature_inside->unit = UNIT_FAHRENHEIT;

	if (a->humidity == NULL) {
		a->humidity = (meteovalue_t *)malloc(sizeof(meteovalue_t));
		if (debug)
			fprintf(stderr, "%s:%d: (meteovalue_t *)malloc(%d)"
				" = %p\n", __FILE__, __LINE__,
				sizeof(meteovalue_t), a->humidity);
	}
	a->humidity->value = 0.;
	a->humidity->max = 0.;
	a->humidity->min = 100.;
	a->humidity->unit = UNIT_NONE;

	if (a->humidity_inside == NULL) {
		a->humidity_inside = (meteovalue_t *)malloc(
			sizeof(meteovalue_t));
		if (debug)
			fprintf(stderr, "%s:%d: (meteovalue_t *)malloc(%d)"
				" = %p\n", __FILE__, __LINE__,
				sizeof(meteovalue_t), a->humidity_inside);
	}
	a->humidity_inside->value = 0.;
	a->humidity_inside->max = 0.;
	a->humidity_inside->min = 100.;
	a->humidity_inside->unit = UNIT_NONE;

	if (a->barometer == NULL) {
		a->barometer = (meteovalue_t *)malloc(sizeof(meteovalue_t));
		if (debug)
			fprintf(stderr, "%s:%d: (meteovalue_t *)malloc(%d)"
				" = %p\n", __FILE__, __LINE__,
				sizeof(meteovalue_t), a->barometer);
	}
	a->barometer->value = 0.;
	a->barometer->max = 0.;
	a->barometer->min = 10000.;
	a->barometer->unit = UNIT_INHG;

	if (a->wind == NULL) {
		a->wind = (wind_t *)malloc(sizeof(wind_t));
		if (debug)
			fprintf(stderr, "%s:%d: (wind_t *)malloc(%d) = %p\n",
				__FILE__, __LINE__, sizeof(wind_t), a->wind);
	}
	a->wind->speed = 0.;
	a->wind->speedmax = 0.;
	a->wind->direction = 0.;
	a->wind->unit = UNIT_MPS;
	a->wind->x = 0.;
	a->wind->y = 0.;

	if (a->rain == NULL) {
		a->rain = (rain_t *)malloc(sizeof(rain_t));
		if (debug)
			fprintf(stderr, "%s:%d: (rain_t *)malloc(%d) = %p\n",
				__FILE__, __LINE__, sizeof(rain_t), a->rain);
	}
	a->rain->rain = 0.;
	a->rain->raintotal = -1.;
	a->rain->unit = UNIT_MM;

	if (a->solar == NULL) {
		a->solar = (meteovalue_t *)malloc(sizeof(meteovalue_t));
		if (debug)
			fprintf(stderr, "%s:%d: (meteovalue_t *)malloc(%d)"
				" = %p\n", __FILE__, __LINE__,
				sizeof(meteovalue_t), a->solar);
	}
	a->solar->value = 0.;
	a->solar->max = 0.;
	a->solar->min = 65535.;
	a->solar->unit = UNIT_WM2;

	if (a->uv == NULL) {
		a->uv = (meteovalue_t *)malloc(sizeof(meteovalue_t));
		if (debug)
			fprintf(stderr, "%s:%d: (meteovalue_t *)malloc(%d)"
				" = %p\n", __FILE__, __LINE__,
				sizeof(meteovalue_t), a->uv);
	}
	a->uv->value = 0.;
	a->uv->max = 0.;
	a->uv->min = 256.;
	a->uv->unit = UNIT_UVINDEX;
}

#define	maxvalue(a, u) unitconvert(a->unit, u, a->max)
#define	minvalue(a, u) unitconvert(a->unit, u, a->min)
#define	value(a, u) unitconvert(a->unit, u, a->value)

void	meteodata_display(FILE *o, meteodata_t *a) {
	fprintf(o, "parameter          value    min    max\n");
	fprintf(o, "temperature out    %5.1f  %5.1f  %5.1f deg C\n",
		value(a->temperature, UNIT_CELSIUS),
		minvalue(a->temperature, UNIT_CELSIUS),
		maxvalue(a->temperature, UNIT_CELSIUS));
	fprintf(o, "temperature in     %5.1f  %5.1f  %5.1f deg C\n",
		value(a->temperature_inside, UNIT_CELSIUS),
		minvalue(a->temperature_inside, UNIT_CELSIUS),
		maxvalue(a->temperature_inside, UNIT_CELSIUS));
	fprintf(o, "humidity out       %5.1f  %5.1f  %5.1f %%\n",
		value(a->humidity, UNIT_NONE),
		minvalue(a->humidity, UNIT_NONE),
		maxvalue(a->humidity, UNIT_NONE));
	fprintf(o, "humidity in        %5.1f  %5.1f  %5.1f %%\n",
		value(a->humidity_inside, UNIT_NONE),
		minvalue(a->humidity_inside, UNIT_NONE),
		maxvalue(a->humidity_inside, UNIT_NONE));
	fprintf(o, "barometer         %6.1f %6.1f %6.1f hPa\n",
		value(a->barometer, UNIT_HPA),
		minvalue(a->barometer, UNIT_HPA),
		maxvalue(a->barometer, UNIT_HPA));
	fprintf(o, "solar radiation   %6.1f %6.1f %6.1f W/m2\n",
		value(a->solar, UNIT_WM2),
		minvalue(a->solar, UNIT_WM2),
		maxvalue(a->solar, UNIT_WM2));
	fprintf(o, "uv radiation       %5.1f  %5.1f  %5.1f index\n",
		value(a->uv, UNIT_UVINDEX),
		minvalue(a->uv, UNIT_UVINDEX),
		maxvalue(a->uv, UNIT_UVINDEX));
	fprintf(o, "wind               %5.1f         %5.1f m/s\n",
		unitconvert(a->wind->unit, UNIT_MPS, a->wind->speed),
		unitconvert(a->wind->unit, UNIT_MPS, a->wind->speedmax));
	fprintf(o, "wind direction     %5.1f\n", a->wind->direction);
	fprintf(o, "                   total    day\n");
	fprintf(o, "rain               %5.1f  %5.1f mm\n",
		unitconvert(a->rain->unit, UNIT_MM, a->rain->rain),
		unitconvert(a->rain->unit, UNIT_MM, a->rain->raintotal));
	fprintf(o, "interval:          %10.6f\n",
		(a->last.tv_sec - a->start.tv_sec)
		+ (a->last.tv_usec - a->start.tv_usec)/1000000.);
	fprintf(o, "\n");
}
