/*
 * meteoupdate.c 
 *
 * read data from the message queue and update the database using the
 * queries
 *
 * (c) 2001 Dr. Andreas Mueller
 *
 * $Id: meteoupdate.c,v 1.3 2002/06/22 15:57:40 afm Exp $
 */
#include <stdlib.h>
#include <stdio.h>
#include <mysql/mysql.h>
#include <meteo.h>
#include <meteodata.h>
#include <sys/types.h>
#include <sys/time.h>
#include <errno.h>
#include <davis.h>
#include <string.h>
#include <sercom.h>
#include <tcpcom.h>
#include <msgque.h>
#include <unistd.h>
#include <printver.h>
#include <mdebug.h>

extern int	optind;
extern char	*optarg;

int	main(int argc, char *argv[]) {
	char		query[4096];
	char		*hostname = "localhost",
			*user = "meteo",
			*password = "public",
			*database = "meteo",
			*keyfile = NULL;
	MYSQL		mysql;
	int		c, mq, l;

	/* parse command line arguments					*/
	while (EOF != (c = getopt(argc, argv, "db:h:p:u:k:V")))
		switch (c) {
		case 'b':
			database = optarg;
			break;
		case 'd':
			debug++;
			break;
		case 'h':
			hostname = optarg;
			break;
		case 'p':
			password = optarg;
			break;
		case 'u':
			user = optarg;
			break;
		case 'k':
			keyfile = optarg;
			break;
		case 'V':
			fprintver(stdout);
			exit(EXIT_SUCCESS);
			break;
		}

	/* get the message queue from the key file			*/
	if (keyfile == NULL) {
		fprintf(stderr, "%s:%d: no key file specified\n", __FILE__,
			__LINE__);
		exit(EXIT_FAILURE);
	}
	mq = msgque_setup(keyfile);
	if (mq < 0) {
		fprintf(stderr, "%s:%d: no msg found\n", __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}

	/* connect to the database					*/
	mysql_init(&mysql);
	if (NULL == mysql_real_connect(&mysql, hostname, user, password,
		database, 0, NULL, 0)) {
		fprintf(stderr, "%s:%d: failed to connect to database\n",
			__FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}

	/* start reading messages from the message queue		*/
	do {
		/* read message from queue				*/
		l = msgque_rcvquery(mq, query, 4096);
		if (l < 0) {
			fprintf(stderr, "%s:%d: problem receiving message: "
				"%s (%d)\n", __FILE__, __LINE__,
				strerror(errno), errno);
			exit(EXIT_FAILURE);
		}

		/* send the update to the database			*/
		if (mysql_query(&mysql, query)) {
			fprintf(stderr, "%s:%d: query failed: %s\n", __FILE__,
				__LINE__, mysql_error(&mysql));
			exit(EXIT_FAILURE);
		}
	} while (1);

	/* close the connection to the database				*/
	mysql_close(&mysql);
	exit(EXIT_SUCCESS);
}
