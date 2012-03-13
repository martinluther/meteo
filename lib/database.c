/*
 * database.c -- open/close the database connection
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: database.c,v 1.6 2003/06/09 07:33:21 afm Exp $
 */
#include <database.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <meteo.h>
#include <mdebug.h>
#include <xmlconf.h>

MYSQL	*mc_opendb(const meteoconf_t *mc, const int mode) {
	const char	*database, *host, *user, *password, *prefix;
	MYSQL		*mysql;

	/* read the database parameters from the configuration file     */
	database = xmlconf_get_abs_string(mc, "/meteo/database/dbname",
		"meteo");
	host = xmlconf_get_abs_string(mc, "/meteo/database/hostname",
		"localhost");
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "connecting to %s@%s", database, host);
	if ((mode & O_WRONLY) || (mode & O_RDWR)) {
		user = xmlconf_get_abs_string(mc, "/meteo/database/writer",
			"meteo");
		password = xmlconf_get_abs_string(mc,
			"/meteo/database/writerpassword", "public");
	} else {
		user = xmlconf_get_abs_string(mc, "/meteo/database/user",
			"meteo");
		password = xmlconf_get_abs_string(mc,
			"/meteo/database/password", "public");
	}

	prefix = xmlconf_get_abs_string(mc, "/meteo/database/prefix", NULL);
	if (prefix == NULL) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "prefix must be set!");
		return NULL;
	}

	/* create a new database structure				*/
	mysql = (MYSQL *)malloc(sizeof(MYSQL));
	if (NULL == mysql) {
		mdebug(LOG_ERR, MDEBUG_LOG, MDEBUG_ERRNO,
			"cannot allocate mysql structure");
		return NULL;
	}

	/* initialize the structure					*/
	mysql_init(mysql);

	/* connect to the database server				*/
	if (NULL == mysql_real_connect(mysql, host, user, password, database,
		0, NULL, 0)) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "cannot connect to database: %s",
			mysql_error(mysql));
		return NULL;
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "connected to database");

	/* return the mysql query					*/
	return mysql;
}

void	mc_closedb(MYSQL *mysql) {
	/* close the database connection				*/
	mysql_close(mysql);

	/* free the structure						*/
	free(mysql);
}
