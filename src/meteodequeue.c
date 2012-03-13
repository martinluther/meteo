/*
 * meteodequeue.c -- fetch update queries from a message queue and send
 *                   them to the database
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: meteodequeue.c,v 1.1 2002/01/18 23:34:31 afm Exp $
 */
#include <meteo.h>
#include <database.h>
#include <msgque.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <daemon.h>
#include <printver.h>

static void	dequeue_one(int mq, MYSQL *mysql) {
	char	buffer[2048];
	int	r;
	
	/* read one message from the message queue			*/
	if (0 > (r = msgque_rcvquery(mq, buffer, sizeof(buffer)))) {
		fprintf(stderr, "%s:%d: failed to read a message\n",
			__FILE__, __LINE__);
		return;
	}

	/* send the query to the database				*/
	if (debug)
		fprintf(stderr, "%s:%d: query being sent to database is '%s'\n",
			__FILE__, __LINE__, buffer);
	if (mysql_query(mysql, buffer)) {
		fprintf(stderr, "%s:%d: query failed: '%s'\n", __FILE__,
			__LINE__, buffer);
	}
}

int	main(int argc, char *argv[]) {
	MYSQL		*mysql = NULL;
	int		c, mq, foreground = 0;
	char		*conffilename = METEOCONFFILE;
	const char	*queuename = NULL;
	char		*pidfilename = "/var/run/meteodequeue.pid";

	/* parse command line						*/
	while (EOF != (c = getopt(argc, argv, "df:Fp:V")))
		switch (c) {
		case 'd':
			debug++;
			break;
		case 'f':
			conffilename = optarg;
			break;
		case 'F':
			foreground = 1;
			break;
		case 'p':
			pidfilename = optarg;
			break;
		case 'V':
			fprintver(stdout);
			exit(EXIT_SUCCESS);
			break;
		}

	/* read the configuration file					*/
	if (NULL == conffilename) {
		fprintf(stderr, "%s:%d: must specify -f <config>\n", __FILE__,
			__LINE__);
		exit(EXIT_FAILURE);
	}
	meteoconfig = mc_readconf(conffilename);
	if (NULL == meteoconfig) {
		fprintf(stderr, "%s:%d: configuration %s invalid\n", __FILE__,
			__LINE__, conffilename);
		exit(EXIT_FAILURE);
	}

	/* daemonize							*/
	if (!foreground) {
		switch (daemonize(pidfilename, NULL)) {
		case -1:
			fprintf(stderr, "%s:%d: daemonizing failed\n", __FILE__,
				__LINE__);
			exit(EXIT_FAILURE);
			break;
		case 0:	/* parent					*/
			exit(EXIT_SUCCESS);
			break;
		default:/* client					*/
			break;
		}
	}

	/* make sure we have a suitable message queue			*/
	queuename = mc_get_string(meteoconfig, "database.msgqueue", queuename);
	if (NULL == queuename) {
		fprintf(stderr, "%s:%d: msg queue name not specified\n",
			__FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}
	if (debug)
		fprintf(stderr, "%s:%d: opening msgqueue '%s'\n", __FILE__,
			__LINE__, queuename);
	if (0 > (mq = msgque_setup(queuename))) {
		fprintf(stderr, "%s:%d: cannot open msgqueue\n", __FILE__,
			__LINE__);
		exit(EXIT_FAILURE);
	}

	/* open the database						*/
	mysql = mc_opendb(meteoconfig, O_WRONLY);
	if (NULL == mysql) {
		fprintf(stderr, "%s:%d: cannot open database, exiting\n",
			__FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}

	/* start the main loop						*/
	while (1) {
		dequeue_one(mq, mysql);
	}

	/* if we ever get to this point, we close the database and	*/
	/* exit cleanly							*/
	msgque_cleanup(mq);
	mc_closedb(mysql);
	exit(EXIT_SUCCESS);
}
