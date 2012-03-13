/*
 * mdebug.c -- meteo debugging subsystem
 *
 * (c) 2002 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: mdebug.c,v 1.3 2002/01/29 20:55:29 afm Exp $
 */
#include <mdebug.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int	debug = 0;

#define	MSGSIZE	10240

static FILE	*logfile = NULL;
static char	*logident = NULL;

struct logfacility_s {
	const char	*name;
	int		facility;
};
#define	NFACILITIES	17
struct logfacility_s	lf[NFACILITIES] = {
	{	"auth",		LOG_AUTH	},
	{	"cron",		LOG_CRON	},
	{	"daemon",	LOG_DAEMON	},
	{	"kern",		LOG_KERN	},
	{	"lpr",		LOG_LPR		},
	{	"mail",		LOG_MAIL	},
	{	"news",		LOG_NEWS	},
	{	"user",		LOG_USER	},
	{	"uucp",		LOG_UUCP	},
	{	"local0",	LOG_LOCAL0	},
	{	"local1",	LOG_LOCAL1	},
	{	"local2",	LOG_LOCAL2	},
	{	"local3",	LOG_LOCAL3	},
	{	"local4",	LOG_LOCAL4	},
	{	"local5",	LOG_LOCAL5	},
	{	"local6",	LOG_LOCAL6	},
	{	"local7",	LOG_LOCAL7	}
};

int	mdebug_setup_file(const char *ident, FILE *lgf) {
	logfile = lgf;
	logident = strdup(ident);
	return 0;
}

int	mdebug_setup(const char *ident, const char *logurl) {
	int		i;
	const char	*logfacility;

	/* handle case of file log urls					*/
	if (0 == strncmp("file://", logurl, 7)) {
		logfile = fopen(logurl + 7, "a");
		if (logfile == NULL)
			logfile = stderr;
		logident = strdup(ident);
		return 0;
	} 

	/* handle case of syslog urls					*/
	if (0 == strncmp("syslog:", logurl, 7)) {
		logfacility = logurl + 7;
		for (i = 0; i < NFACILITIES; i++) {
			if (0 == strcmp(logfacility, lf[i].name)) {
				openlog(ident, LOG_PID, lf[i].facility);
				return 0;
			}
		}
	}

	/* if we don't get to this point, then either the log url is 	*/
	/* illegal, or the facility does not exist			*/
	errno = ESRCH;
	return -1;
}

void	vmdebug(int loglevel, const char *file, int line, int flags,
	const char *format, va_list ap) {
	char		msgbuffer[MSGSIZE], msgbuffer2[MSGSIZE],
			prefix[MSGSIZE], tstp[MSGSIZE];
	time_t		t;
	struct tm	*tmp;
	int		localerrno;

	/* format the message content					*/
	localerrno = errno;
	vsnprintf(msgbuffer2, sizeof(msgbuffer2), format, ap);
	if (flags & MDEBUG_ERRNO)
		snprintf(msgbuffer, sizeof(msgbuffer), "%s: %s (%d)",
			msgbuffer2, strerror(localerrno), localerrno);
	else
		strcpy(msgbuffer, msgbuffer2);

	/* add prefix depending on file or syslog, and send to dest	*/
	if (logfile != NULL) {
		/* compute a time stamp					*/
		t = time(NULL);
		tmp = localtime(&t);
		strftime(tstp, sizeof(tstp), "%b %e %H:%M:%S", tmp);

		/* compute the format prefix				*/
		if (flags & MDEBUG_NOFILELINE)
			snprintf(prefix, sizeof(prefix), "%s %s[%d]:",
				tstp, logident, getpid());
		else
			snprintf(prefix, sizeof(prefix), "%s %s[%d] %s:%d:",
				tstp, logident, getpid(), file, line);

		/* write the message to the logfile			*/
		fprintf(logfile, "%s %s\n", prefix, msgbuffer);
		fflush(logfile);
	} else {
		if (MDEBUG_NOFILELINE & flags)
			syslog(loglevel, "%s", msgbuffer);
		else
			syslog(loglevel, "%s:%d: %s", file, line, msgbuffer);
	}
}

void	mdebug(int loglevel, const char *file, int line, int flags,
	const char *format, ...) {
	va_list	ap;
	va_start(ap, format);
	vmdebug(loglevel, file, line, flags, format, ap);
	va_end(ap);
}
