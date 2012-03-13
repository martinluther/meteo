/*
 * average.h -- compute averages of meteo parameters for a given date
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: average.h,v 1.2 2003/06/06 15:11:05 afm Exp $
 */
#ifndef _AVERAGE_H
#define _AVERAGE_H

#include <mysql/mysql.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

extern int	average_fake;
extern int	add_average(MYSQL *mysql, const time_t now, const int interval,
			const char *station);
extern int	have_average(MYSQL *mysql, const time_t now, const int interval,
			const char *station);

#ifdef __cplusplus
}
#endif

#endif /* _AVERAGE_H */
