/*
 * com.c -- common data structures for communcation
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: com.c,v 1.4 2002/11/24 19:48:01 afm Exp $
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <crc.h>
#include <com.h>
#include <meteo.h>
#include <sys/time.h>
#include <mdebug.h>
#include <fcntl.h>
#include <string.h>
#include <xmlconf.h>

static int	com_get_char(meteocom_t *m);
static int	com_get_char_timed(meteocom_t *m, struct timeval *tv);
static int	com_put_char(meteocom_t *m, unsigned char c);

/*
 * get an put functions, public API to the meteocom object
 */
int	get_char(meteocom_t *m) {
	int	c;
	c = m->get_char(m);
	if (debug > 1)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "got one char %02x ('%c')",
			c, ((c >= ' ') && (c < 0x7f)) ? c : '?');
	return c;
}

int	get_char_timed(meteocom_t *m, struct timeval *tv) {
	int	c;
	c = m->get_char_timed(m, tv);
	if (debug > 1)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "got one char %02x ('%c')",
			c, ((c >= ' ') && (c < 0x7f)) ? c : '?');
	return c;
}

int	put_char(meteocom_t *m, unsigned char c) {
	if (debug > 1)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "sending one char %02x ('%c')",
			c, ((c >= ' ') && (c < 0x7f)) ? c : '?');
	return m->put_char(m, c);
}

int	put_string(meteocom_t *m, char *s) {
	char	*p = s;
	while (*p)
		put_char(m, *p++);
	return 0;
}

int	put_unsigned(meteocom_t *m, unsigned short s) {
	put_char(m, s & 0xff);
	return put_char(m, s >> 8);
}

/*
 * get_acknowledge -- reads characters from the serial interface, skipping
 *                    all characters different from an ACK character or the
 *                    string "OK\n\r", but do this for at most 5 seconds
 */
int	get_acknowledge_timed(meteocom_t *m, struct timeval *tvp) {
	int	timepassed = 0, c, ackfound = 0, idx = 0;
	char	*expect = "\r\n\rOK\n\r";

	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "waiting for ACK for %d.%06d "
			"sec", tvp->tv_sec, tvp->tv_usec);
	do {
		c = get_char_timed(m, tvp);
		if (c < 0) {
			timepassed = 1;
		} else {
			if (c == ACK)
				ackfound = 1;
			if (c == expect[idx])
				idx++;
			if (idx == strlen(expect))
				ackfound = 1;
		}
		
	} while ((timepassed != 1) && (ackfound != 1));
	if (ackfound == 1)
		return 0;
	return -1;
}
int	get_acknowledge(meteocom_t *m) {
	struct timeval	tv;
	tv.tv_sec = 1000; tv.tv_usec = 0;
	return get_acknowledge_timed(m, &tv);
}

int	get_buffer(meteocom_t *m, unsigned char *b, int n) {
	int	i, c;
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"looking for buffer of %d bytes", n);
	crc_start(&m->crc);
	for (i = 0; i < n; i++) {
		c = get_char(m);
		b[i] = c;
		crc_addbyte(&m->crc, c);
	}
	if (debug) {
		char	*buffer = (char *)malloc(n * 3 + 5);
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"(char *)malloc(%d) = %p\n", n * 3 + 5, buffer);
		for (i = 0; i < n; i++)
			sprintf(buffer + 3 * i, "%02x ", b[i]);
		
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "received data: %s\n", buffer);
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "free((char *)%p)", buffer);
		free(buffer);
	}
	return 0;
}

/*
 * default constructor and destructor
 */
meteocom_t	*com_new(meteoconf_t *mc)  {
	meteocom_t	*result;
	result = (meteocom_t *)malloc(sizeof(meteocom_t));
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"(meteocom_t *)malloc(%d) = %p",
			sizeof(meteocom_t), result);
	result->flags = 0;
	result->private = NULL;
	result->put_char = com_put_char;
	result->get_char = com_get_char;
	result->get_char_timed = com_get_char_timed;
	if (NULL != mc) {
		if (0 == strcmp("Vantage", xmlconf_get_string(mc,
			"", "type", NULL, 0, ""))) {
			result->flags |= COM_VANTAGE;
		}
	}
	return result;
}
void	com_free(meteocom_t *m) {
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "free((meteocom_t *)%p)", m);
	free(m);
}

/*
 * the private data contains the file descriptor, encode this fact in a
 * special function
 */
static int	com_get_fd(meteocom_t *m) {
	if (NULL == m)
		return -1;
	return *(int *)m->private;
}

/*
 * generic transfer functions for private data as a file descriptor
 */
static int	com_get_char(meteocom_t *m) {
	unsigned char	b[1];
	if (read(com_get_fd(m), b, 1) != 1) {
		return -1;
	}
	return b[0];
}

static void	normalize_timeval(struct timeval *tv) {
	if (tv->tv_usec < 0) {
		tv->tv_usec += 1000000;
		tv->tv_sec -= 1;
	}
}

static int	com_get_char_timed(meteocom_t *m, struct timeval *tv) {
	struct timeval	start, end, delay;
	fd_set		readfds;
	int		fd, result = -1;
	unsigned char	b[1];

	/* select is not guaranteed to return the remaining time	*/
	gettimeofday(&start, NULL);
	memcpy(&delay, tv, sizeof(struct timeval));

	/* select on the file descriptor				*/
	fd = com_get_fd(m);
	FD_ZERO(&readfds);
	FD_SET(fd, &readfds);
	switch(select(fd + 1, &readfds, NULL, NULL, &delay)) {
	case 0:
		/* a timeout happend					*/
		tv->tv_sec = tv->tv_usec = 0;
		goto timeout;
	case 1:
		/* a character is ready for reading, read it		*/
		if (read(fd, b, 1) == 1) {
			result = b[0];
		}
		break;
	default:
		/* some other error happend				*/
		break;
	}

	/* compute the remaining time					*/
	gettimeofday(&end, NULL);
	delay.tv_sec = end.tv_sec - start.tv_sec;
	delay.tv_usec = end.tv_usec - start.tv_usec;
	normalize_timeval(&delay);
	tv->tv_sec -= delay.tv_sec;
	tv->tv_usec -= delay.tv_usec;
	normalize_timeval(tv);

timeout:
	/* return the result: character or -1				*/
	return result;
}

static int	com_put_char(meteocom_t *m, unsigned char c) {
	if (1 != write(com_get_fd(m), &c, 1))
		return -1;
	return 0;
}

/*
 * com_drain	wait some time for input on the connection, read it all
 *              and throw it away
 */
#define	LOOPLIMIT	10
int	com_drain(meteocom_t *m, const struct timeval *timeout) {
	struct timeval	tv;
	int		loops = 0, fd, bytes, totalbytes = 0;
	long		flags;
	char		buffer[1024];

	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "draining for %d.%06d secs",
			timeout->tv_sec, timeout->tv_usec);

	/* first let some time pass					*/
	memcpy(&tv, timeout, sizeof(tv));
	if (0 != select(0, NULL, NULL, NULL, &tv)) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, MDEBUG_ERRNO,
			"select problem in drain");
	}

	/* get the file descriptor for this meteocom_t structure	*/
	fd = com_get_fd(m);

	/* find out whether the nowait flag is set on the file		*/
	flags = fcntl(fd, F_GETFL);

	/* set the nowait flag if necessary				*/
	if (!(flags & O_NONBLOCK)) {
		fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	}

	/* read a large buffer of junk					*/
	while (loops < LOOPLIMIT) {
		if ((bytes = read(fd, buffer, sizeof(buffer))) <= 0)
			break;
		totalbytes += bytes;
		loops++;
	}
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "%d bytes drained");

	/* restore the wait flag, if previously set the nowait flag	*/
	if (!(flags & O_NONBLOCK)) {
		fcntl(fd, F_SETFL, flags);
	}
	return 0;
}
