/*
 * meteodequeue.cc -- fetch update queries from a message queue and send
 *                    them to the database
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: meteodequeue.cc,v 1.3 2003/06/12 23:29:46 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <meteo.h>
#include <database.h>
#include <msgque.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <daemon.h>
#include <printver.h>
#include <mdebug.h>

static void	dequeue_one(msgque_t *mq, MYSQL *mysql) {
	char	buffer[2048];
	int	r;
	
	/* read one message from the message queue			*/
	if (0 > (r = msgque_rcvquery(mq, buffer, sizeof(buffer)))) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "failed to read a message");
		return;
	}

	/* send the query to the database				*/
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"query being sent to database is '%s'", buffer);
	if (mysql_query(mysql, buffer)) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "query '%s' failed: %s", buffer,
			mysql_error(mysql));
	}
}

static void	usage(void) {
printf(
"usage: meteodequeue [ -dFVh? ] [ -l logurl ] [ -p pidfile ] [ -f conffile ]\n"
" -d             increase debug level\n"
" -F             stay in foreground (for debuggin)\n"
" -V             display version and exit\n"
" -h, -?         display this help text and exit\n"
" -l logurl      write log messages to this log url (see meteo(1))\n"
" -p pidfile     write pid to pidfile\n"
" -c conffile    use conffile, see meteo.xml(5)\n"
);
}

int	main(int argc, char *argv[]) {
	MYSQL		*mysql = NULL;
	int		c, foreground = 0;
	msgque_t	*mq = NULL;
	char		*conffilename = METEOCONFFILE;
	const char	*queuename = NULL;
	char		*pidfilename = "/var/run/meteodequeue.pid";
	meteoconf_t	*mc;

	/* parse command line						*/
	while (EOF != (c = getopt(argc, argv, "l:df:Fp:Vh?")))
		switch (c) {
		case 'l':
			if (mdebug_setup("meteodequeue", optarg) < 0) {
				fprintf(stderr, "%s: cannot init log\n",
					argv[0]);
				exit(EXIT_FAILURE);
			}
			break;
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
		case 'h':
		case '?':
			usage(); exit(EXIT_SUCCESS);
			break;
		case 'V':
			fprintver(stdout);
			exit(EXIT_SUCCESS);
			break;
		}

	/* read the configuration file					*/
	if (NULL == conffilename) {
		mdebug(LOG_CRIT, MDEBUG_LOG, 0, "must specify -f <config>");
		exit(EXIT_FAILURE);
	}
	mc = xmlconf_new(conffilename, "");
	if (NULL == mc) {
		mdebug(LOG_CRIT, MDEBUG_LOG, 0, "configuration %s invalid",
			conffilename);
		exit(EXIT_FAILURE);
	}

	/* daemonize							*/
	if (!foreground) {
		switch (daemonize(pidfilename, NULL)) {
		case -1:
			mdebug(LOG_CRIT, MDEBUG_LOG, 0, "daemonizing failed");
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
	queuename = xmlconf_get_abs_string(mc, "/meteo/database/msgqueue",
		queuename);
	if (NULL == queuename) {
		mdebug(LOG_CRIT, MDEBUG_LOG, 0, "msg queue name not specified");
		exit(EXIT_FAILURE);
	}
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "opening msgqueue '%s'",
			queuename);
	if (NULL == (mq = msgque_setup(queuename))) {
		mdebug(LOG_CRIT, MDEBUG_LOG, 0, "cannot open msgqueue");
		exit(EXIT_FAILURE);
	}

	/* open the database						*/
	mysql = mc_opendb(mc, O_WRONLY);
	if (NULL == mysql) {
		mdebug(LOG_CRIT, MDEBUG_LOG, 0,
			"cannot open database, exiting");
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
