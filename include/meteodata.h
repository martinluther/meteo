/*
 * meteoaccess.h -- abstract meteo data interface
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: meteodata.h,v 1.2 2002/08/24 14:56:21 afm Exp $
 */
#ifndef _METEODATA_H
#define _METEODATA_H

#include <sys/types.h>
#include <time.h>
#include <com.h>
#include <conversions.h>
#include <stdio.h>

#define	METEOVALUE_HASVALUE	 1 << 8
#define	METEOVALUE_HASMAX	 2 << 8
#define	METEOVALUE_HASMIN	 4 << 8
#define	METEOVALUE_HASDIRECTION	 8 << 8
#define METEOVALUE_HASTOTAL	16 << 8

#define	METEOVALUE_TEMPERATURE		1
#define	METEOVALUE_TEMPERATURE_INSIDE	2
#define	METEOVALUE_HUMIDITY		3
#define	METEOVALUE_HUMIDITY_INSIDE	4
#define	METEOVALUE_BAROMETER		5
#define METEOVALUE_SOLAR		6
#define	METEOVALUE_UV			7

typedef struct meteovalue {
	double	value;
	double	max;
	int	unit;
	time_t	maxtime;
	double	min;
	time_t	mintime;
	int	flags;
	double	run;
} meteovalue_t;

typedef	struct wind {
	double	direction;
	double	speed;
	double	speedmax;
	int	unit;
	time_t	maxtime;
	int	flags;
	double	x;
	double	y;
	int	directions[16];
} wind_t;

typedef struct rain {
	int	unit;
	double	rain;
	double	raintotal;
	int	flags;
} rain_t;

#define	BAROTREND_UNKNOWN		-1
#define BAROTREND_FALLING_RAPIDLY	0
#define BAROTREND_FALLING_SLOWLY	1
#define	BAROTREND_STEADY		2
#define BAROTREND_RISING_SLOWLY		3
#define BAROTREND_RISING_RAPIDLY	4

typedef struct meteodata {
	meteovalue_t	*temperature;
	meteovalue_t	*temperature_inside;
	meteovalue_t	*humidity;
	meteovalue_t	*humidity_inside;
	meteovalue_t	*barometer;
	int		barotrend;
	wind_t		*wind;
	rain_t		*rain;
	meteovalue_t	*uv;
	meteovalue_t	*solar;
	int		samples;
	struct timeval	start;
	struct timeval	last;
} meteodata_t;

typedef struct meteoaccess {
	meteocom_t	*m;
	meteovalue_t	*(*get_temperature)();
	meteovalue_t	*(*get_temperature_inside)();
	meteovalue_t	*(*get_humidity)();
	meteovalue_t	*(*get_humidity_inside)();
	meteovalue_t	*(*get_barometer)();
	int		(*get_barotrend)();
	wind_t		*(*get_wind)();
	rain_t		*(*get_rain)();
	meteodata_t	*(*get_data)();
	meteodata_t	*(*get_update)();
	void		*private;
} meteoaccess_t;

#define	get_temperature(m)		(m->get_temperature(m))
#define	get_temperature_inside(m)	(m->get_temperature_inside(m))
#define	get_humidity(m)			(m->get_humidity(m))
#define	get_humidity_inside(m)		(m->get_humidity_inside(m))
#define	get_barometer(m)		(m->get_barometer(m))
#define get_barotrend(m)		(m->get_barotrend(m))
#define	get_rain(m)			(m->get_rain(m))
#define	get_wind(m)			(m->get_wind(m))
#define get_data(m)			(m->get_data(m))
#define get_update(m)			(m->get_update(m))

extern void	meteovalue_set(meteovalue_t *m, double value);
extern void	meteowind_set(wind_t *m, double speed, double direction);
extern void	meteorain_set(rain_t *r, double value);
extern meteodata_t	*meteodata_new(void);
extern void	meteodata_free(meteodata_t *a);
extern void	meteodata_update(meteodata_t *a,
		double temperature, double temperature_inside,
		double humidity, double humidity_inside,
		double barometer, int barotrend,
		double speed, double direction, double rain, double solar,
		double uv);
extern void	meteodata_start(meteodata_t *a);
extern void	meteodata_display(FILE *o, meteodata_t *a);

#endif /* _METEODATA_H */
