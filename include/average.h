/*
 * average.h -- compute averages of meteo parameters for a given date
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: average.h,v 1.1 2002/01/18 23:34:25 afm Exp $
 */
#ifndef _AVERAGE_H
#define _AVERAGE_H

#include <mysql/mysql.h>
#include <time.h>

extern int	average_fake;
extern int	add_average(MYSQL *mysql, const time_t now, const int interval,
			const char *station);
extern int	have_average(MYSQL *mysql, const time_t now, const int interval,
			const char *station);

#endif /* _AVERAGE_H */
