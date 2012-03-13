/*
 * meteosocket.c -- create a socket, format messages sent to it, or parse
 *                  messages received on it
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: meteosocket.c,v 1.1 2001/12/24 14:39:46 afm Exp $
 */
#include <meteosocket.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

int	meteosocket(char *socketpath, int mode) {
	int			s;
	struct sockaddr_un	su;
	struct stat		sb;

	/* check whether the path length is acceptable			*/
	if (strlen(socketpath) > (sizeof(su.sun_path) - 1)) {
		fprintf(stderr, "%s:%d: socketpath to long, %d > %d\n",
			__FILE__, __LINE__, strlen(socketpath),
			sizeof(su.sun_path) - 1);
		errno = E2BIG;
		return -1;
	}

	/* check whether the socket already exists			*/
	if (stat(socketpath, &sb) >= 0) {
		fprintf(stderr, "%s:%d: warning: file %s exists, deleting\n",
			__FILE__, __LINE__, socketpath);
		if (unlink(socketpath) < 0) {
			fprintf(stderr, "%s:%d: cannot delete file %s: "
				"%s (%d)\n", __FILE__, __LINE__, socketpath,
				strerror(errno), errno);
			return -1;
		}
	}

	/* create a Unix domain datagram socket				*/
	if ((s = socket(PF_UNIX, SOCK_DGRAM, 0)) < 0) {
		fprintf(stderr, "%s:%d: socket call fails: %s (%d)\n",
			__FILE__, __LINE__, strerror(errno), errno);
		return s;
	}

	/* formulate the socket address based on the socket path	*/
	su.sun_family = AF_UNIX;
	strncpy(su.sun_path, socketpath, sizeof(su.sun_path));

	/* further actions depend on the mode				*/
	switch (mode) {
	case METEO_SERVER:
		/* bind to the socket					*/
		if (bind(s, (struct sockaddr *)&su, sizeof(su)) < 0) {
			fprintf(stderr, "%s:%d: bind to socket failed: "
				"%s (%d)\n", __FILE__, __LINE__,
				strerror(errno), errno);
			return -1;
		}

		break;
	case METEO_CLIENT:
		/* connect to the socket				*/
		if (connect(s, (struct sockaddr *)&su, sizeof(su)) < 0) {
			fprintf(stderr, "%s:%d: cannot connect: %s (%d)\n",
				__FILE__, __LINE__, strerror(errno), errno);
			return -1;
		}
		break;
	}

	return s;
}

/*
 * the following functions format or parse meteovalue_t, wind_t and rain_t
 * structures
 */
static char	*meteovalue2str(char *buffer, meteovalue_t *meteovalue) {
	return buffer;
}
static char	*str2meteovalue(meteovalue_t **meteovalue, char *buffer) {
	return buffer;
}	

static char	*rain2str(char *buffer, rain_t *rain) {
	return buffer;
}
static char	*str2rain(rain_t **rain, char *buffer) {
	return buffer;
}

static char	*wind2str(char *buffer, wind_t *wind) {
	return buffer;
}
static char	*str2wind(wind_t **wind, char *buffer) {
	return buffer;
}

static char	*timeval2str(char *buffer, struct timeval *tv) {
	return buffer;
}
static char	*str2timeval(struct timeval *tv, char *buffer) {
	return buffer;
}

static char	*int2str(char *buffer, int i) {
	buffer += sprintf(buffer, "%d ", i);
	return buffer;
}
static char	*str2int(int *i, char *buffer) {
	
	return buffer;
}


static char	*char2str(char *buffer, char *s) {
	buffer += sprintf(buffer, "%s ", s);
	return buffer;
}
static char	*str2char(char **s, char *buffer) {
	char	*b;
	int	l;
	b = (char *)alloca(strlen(buffer) + 1);
	if (1 != sscanf(buffer, "%s ", b)) {
		/* we have a problem */;
		return NULL;
	}
	l = strlen(b);
	if (s == NULL) {
		*s = strdup(b);
	} else {
		strcpy(*s, b);
	}
	buffer += l;
	return buffer;
}


int	meteorecv(int s, meteodata_t *md, char **station) {
	char	msg[10240];
	char	*p = msg;
	int	l = -1;

	/* receive a message						*/

	/* analyze the message						*/
	p = str2meteovalue(&md->temperature, p);
	p = str2meteovalue(&md->temperature_inside, p);
	p = str2meteovalue(&md->humidity, p);
	p = str2meteovalue(&md->humidity_inside, p);
	p = str2meteovalue(&md->barometer, p);
	p = str2wind(&md->wind, p);
	p = str2rain(&md->rain, p);
	p = str2timeval(&md->start, p);
	p = str2timeval(&md->last, p);
	p = str2int(&md->samples, p);
	p = str2char(station, p);

	/* return the size of the message received			*/
	return l;
}

int	meteosend(int s, meteodata_t *md, char *station) {
	char	msg[10240];
	char	*p = msg;
	int	l;

	/* format the contents of the members				*/
	p = meteovalue2str(p, md->temperature);
	p = meteovalue2str(p, md->temperature_inside);
	p = meteovalue2str(p, md->humidity);
	p = meteovalue2str(p, md->humidity_inside);
	p = meteovalue2str(p, md->barometer);
	p = wind2str(p, md->wind);
	p = rain2str(p, md->rain);
	p = timeval2str(p, &md->start);
	p = timeval2str(p, &md->last);
	p = int2str(p, md->samples);
	p = char2str(p, station);

	/* find the message size					*/
	l = p - msg;

	/* send the message to the other side				*/
	if (send(s, msg, l, 0) < l) {
		fprintf(stderr, "%s:%d: sending message failed: %s (%d)\n",
			__FILE__, __LINE__, strerror(errno), errno);
		return -1;
	}
	return 0;
}
