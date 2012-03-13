/*
 * database.h -- connect to the database based on the configuration file
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: database.h,v 1.2 2002/11/24 19:48:01 afm Exp $
 */
#ifndef _DATABASE_H
#define _DATABASE_H

#include <xmlconf.h>
#include <mysql/mysql.h>
#include <fcntl.h>	/* for mode constants */

extern MYSQL	*mc_opendb(const meteoconf_t *mc, const int mode);
extern void	mc_closedb(MYSQL *);

#endif /* _DATABASE_H */
