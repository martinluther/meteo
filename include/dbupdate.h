/*
 * dbupdate.h  -- perform an update to the database
 *
 * (c) 2001 Dr. Andreas Mueller
 *
 * $Id: dbupdate.h,v 1.4 2003/05/29 20:42:09 afm Exp $
 */
#ifndef _DBUPDATE_H
#define _DBUPDATE_H

#include <meteo.h>
#include <meteodata.h>
#include <mysql/mysql.h>
#include <msgque.h>

#define DEST_NONE	0
#define	DEST_MYSQL	1
#define	DEST_MSGQUE	2
#define	DEST_FILE	3
typedef struct	dest_s {
	int	type;
	char	*name;
	union {
		MYSQL		*mysql;
		msgque_t	*msgque;
		int		file;
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
