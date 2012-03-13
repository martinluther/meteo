/*
 * tcpcom.c -- tcp based communications. works similarly to serial
 *             interfaces, but simpler, as the serial driver is something
 *             quite complicated
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: tcpcom.c,v 1.1 2002/01/18 23:34:30 afm Exp $
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

meteocom_t	*tcpcom_new(char *url) {
	meteocom_t		*r = 0;
	struct hostent		*hp;
	struct sockaddr_in	sa;
	char			*hostname, *portstring, *p;
	unsigned short		port;

	/* allocate memory for the serial device file descriptor	*/
	r = com_new();
	r->private = malloc(sizeof(int));
	if (debug)
		fprintf(stderr, "%s:%d: (int *)malloc(%d) = %p\n",
			__FILE__, __LINE__, sizeof(int), r->private);

	/* parse the URL into hostname and port				*/
	if (strncmp(url, "tcp://", 6) != 0) {
		fprintf(stderr, "%s:%d: wrong method in URL\n", __FILE__,
			__LINE__);
		goto err;
	}
	hostname = &url[6];
	p = strchr(hostname, ':');
	if (p == NULL) {
		fprintf(stderr, "%s:%d: URL does not contain port part\n",
			__FILE__, __LINE__);
		goto err;
	}
	*p = '\0';
	p++;
	portstring = p;
	if (NULL != (p = strchr(portstring, '/')))
		*p = '\0';
	port = atoi(portstring);
	if ((port <= 0) || (port > 65535)) {
		fprintf(stderr, "%s:%d: illegal port specification %s\n",
			__FILE__, __LINE__, portstring);
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
		fprintf(stderr, "%s:%d: free((meteocom_t *)%p)\n",
			__FILE__, __LINE__, m);
	free(m->private);
	com_free(m);
}
