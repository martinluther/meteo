/*
 * database.h -- connect to the database based on the configuration file
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: database.h,v 1.4 2003/06/12 00:45:15 afm Exp $
 */
#ifndef _DATABASE_H
#define _DATABASE_H

#include <xmlconf.h>
#include <mysql.h>
#include <fcntl.h>	/* for mode constants */

#ifdef __cplusplus
extern "C" {
#endif

extern MYSQL	*mc_opendb(const meteoconf_t *mc, const int mode);
extern void	mc_closedb(MYSQL *);

#ifdef __cplusplus
}
#endif

#endif /* _DATABASE_H */
