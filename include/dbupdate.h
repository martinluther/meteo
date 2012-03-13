/*
 * dbupdate.h  -- perform an update to the database
 *
 * (c) 2001 Dr. Andreas Mueller
 *
 * $Id: dbupdate.h,v 1.2 2002/11/18 02:36:42 afm Exp $
 */
#ifndef _DBUPDATE_H
#define _DBUPDATE_H

#include <meteo.h>
#include <meteodata.h>
#include <mysql/mysql.h>

#define DEST_NONE	0
#define	DEST_MYSQL	1
#define	DEST_MSGQUE	2
typedef struct	dest_s {
	int	type;
	char	*name;
	union {
		MYSQL	*mysql;
		int	msgque;
	} destdata;
} dest_t;

/*
 * the dbupdate function takes the meteodata record and sends it to the
 * the mysql database.
 */
extern dest_t	*dest_new(void);
extern void	dest_free(dest_t *);
extern int	dbupdate(dest_t *ddp, meteodata_t *md, const char *station);

#endif /* _DBUPDATE_H */
