/*
 * database.c -- open/close the database connection
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: database.c,v 1.2 2001/12/26 22:10:45 afm Exp $
 */
#include <database.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <meteo.h>

MYSQL	*mc_opendb(const mc_node_t *lmeteoconfig, const int mode) {
	const char	*database, *host, *user, *password, *prefix;
	MYSQL		*mysql;

	/* read the database parameters from the configuration file     */
	database = mc_get_string(lmeteoconfig, "database.dbname", "meteo");
	host = mc_get_string(lmeteoconfig, "database.hostname", "localhost");
	if ((mode & O_WRONLY) || (mode & O_RDWR)) {
		user = mc_get_string(lmeteoconfig, "database.writer", "meteo");
		password = mc_get_string(lmeteoconfig,
			"database.writerpassword", "public");
	} else {
		user = mc_get_string(lmeteoconfig, "database.user", "meteo");
		password = mc_get_string(lmeteoconfig, "database.password",
			"public");
	}

	prefix = mc_get_string(lmeteoconfig, "database.prefix", NULL);
	if (prefix == NULL) {
		fprintf(stderr, "%s:%d: prefix must be set!\n", __FILE__,
			__LINE__);
		return NULL;
	}

	/* create a new database structure				*/
	mysql = (MYSQL *)malloc(sizeof(MYSQL));
	if (NULL == mysql) {
		fprintf(stderr, "%s:%d: cannot allocate mysql structure: "
			"%s (%d)\n", __FILE__, __LINE__,
			strerror(errno), errno);
		return NULL;
	}

	/* initialize the structure					*/
	mysql_init(mysql);

	/* connect to the database server				*/
	if (NULL == mysql_real_connect(mysql, host, user, password, database,
		0, NULL, 0)) {
		fprintf(stderr, "%s:%d: cannot connect to database\n",
			__FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}
	if (debug)
		fprintf(stderr, "%s:%d: connected to database\n",
			__FILE__, __LINE__);

	/* return the mysql query					*/
	return mysql;
}

void	mc_closedb(MYSQL *mysql) {
	/* close the database connection				*/
	mysql_close(mysql);

	/* free the structure						*/
	free(mysql);
}
