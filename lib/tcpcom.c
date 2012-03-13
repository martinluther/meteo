/*
 * tcpcom.c -- tcp based communications. works similarly to serial
 *             interfaces, but simpler, as the serial driver is something
 *             quite complicated
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: tcpcom.c,v 1.3 2002/11/24 23:16:48 afm Exp $
 */
#include <config.h>
#include <tcpcom.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <meteo.h>
#include <mdebug.h>

meteocom_t	*tcpcom_new(meteoconf_t *mc, char *url) {
	meteocom_t		*r = 0;
	struct hostent		*hp;
	struct sockaddr_in	sa;
	char			*hostname, *portstring, *p;
	unsigned short		port;

	/* allocate memory for the serial device file descriptor	*/
	r = com_new(mc);
	r->private = malloc(sizeof(int));
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "(int *)malloc(%d) = %p",
			sizeof(int), r->private);

	/* parse the URL into hostname and port				*/
	if (strncmp(url, "tcp://", 6) != 0) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "wrong method in URL");
		goto err;
	}
	hostname = &url[6];
	p = strchr(hostname, ':');
	if (p == NULL) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0,
			"URL does not contain port part");
		goto err;
	}
	*p = '\0';
	p++;
	portstring = p;
	if (NULL != (p = strchr(portstring, '/')))
		*p = '\0';
	port = atoi(portstring);
	if ((port <= 0) || (port > 65535)) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "illegal port specification %s",
			portstring);
		goto err;
	}

	/* resolve the hostname						*/
	hp = gethostbyname(hostname);
	if (hp == NULL)
		goto err;

	/* create the socket address					*/
	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	memcpy(&sa.sin_addr, hp->h_addr, 4);
	
	/* create a socket						*/
	*(int *)r->private = socket(PF_INET, SOCK_STREAM, 0);
	if (*(int *)r->private < 0)
		goto err;

	/* connect the socket to the remote site			*/
	if (connect(*(int *)r->private, (struct sockaddr *)&sa, sizeof(sa)) < 0)
	{
		goto err;
	}

	/* generic communcation functions are ok			*/

	return r;

	/* error return							*/
err:
	return NULL;
}

void	tcpcom_free(meteocom_t *m) {
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "free((meteocom_t *)%p)",
			m);
	free(m->private);
	com_free(m);
}
