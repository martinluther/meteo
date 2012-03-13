/*
 * database.h -- connect to the database based on the configuration file
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: database.h,v 1.3 2001/12/26 22:10:45 afm Exp $
 */
#ifndef _DATABASE_H
#define _DATABASE_H

#include <mconf.h>
#include <mysql/mysql.h>
#include <fcntl.h>	/* for mode constants */

extern MYSQL	*mc_opendb(const mc_node_t *, const int mode);
extern void	mc_closedb(MYSQL *);

#endif /* _DATABASE_H */
