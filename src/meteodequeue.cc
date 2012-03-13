/*
 * meteodequeue.cc -- fetch update queries from a message queue and send
 *                    them to the database
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: meteodequeue.cc,v 1.6 2003/10/14 23:47:46 afm Exp $
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <Daemon.h>
#include <printver.h>
#include <mdebug.h>
#include <Configuration.h>
#include <QueryProcessor.h>
#include <MsgDequeuer.h>
#include <MeteoException.h>

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
	int		c, foreground = 0;
	std::string	conffilename(METEOCONFFILE);
	std::string	pidfilename("/var/run/meteodequeue.pid");

	// parse command line						
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
			conffilename = std::string(optarg);
			break;
		case 'F':
			foreground = 1;
			break;
		case 'p':
			pidfilename = std::string(optarg);
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

	// read the configuration file					
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "config file is %s",
		conffilename.c_str());
	meteo::Configuration	conf(conffilename);

	// daemonize							
	meteo::Daemon	d(pidfilename, "", foreground);

	// make sure we have a suitable message queue			
	std::string	queuename;
	queuename = conf.getString("/meteo/database/msgqueue", queuename);
	if (queuename.empty()) {
		mdebug(LOG_CRIT, MDEBUG_LOG, 0, "msg queue name not specified");
		exit(EXIT_FAILURE);
	}
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "opening msgqueue '%s'",
			queuename.c_str());
	meteo::MsgDequeuer	mq(queuename);

	// open the database						
	meteo::QueryProcessor	qp(true);

	// start the main loop						
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "starting main loop");
	while (1) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"waiting for data on msgqueue");
		try {
			qp.perform(mq());
		} catch (meteo::MeteoException& me) {
			mdebug(LOG_ERR, MDEBUG_LOG, 0, "exception: %s, %s",
				me.getReason().c_str(),
				me.getAddinfo().c_str());
		}
	}

	// if we ever get to this point, we close the database and	
	// exit cleanly							
	exit(EXIT_SUCCESS);
}
