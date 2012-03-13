/*
 * msgque.c -- handle message queues for asynchronous updates
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: msgque.c,v 1.1 2002/01/18 23:34:30 afm Exp $
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <msgque.h>
#include <sys/msg.h>
#include <meteo.h>
#include <time.h>

#define QUERYMSG_SIZE	4096

/*
 * msgque_setup
 *
 * create a message queue (if it does not exist yet)
 */
int	msgque_setup(const char *filename) {
	key_t	mqkey;
	int	rc;

	/* convert the filename to a key				*/
	mqkey = ftok(filename, 1);
	if (mqkey == (key_t)-1) {
		if (debug)
			fprintf(stderr, "%s:%d: cannot convert key: %s (%d)\n",
				__FILE__, __LINE__, strerror(errno), errno);
		return -1;
	}

	/* get the msgqueue for the key					*/
	rc = msgget(mqkey, IPC_CREAT | 0600);
	if (rc < 0) {
		fprintf(stderr, "%s:%d: cannot get message queue: %s (%d)\n",
			__FILE__, __LINE__, strerror(errno), errno);
		return -1;
	}
	if (debug)
		fprintf(stderr, "%s:%d: msgque id %d allocated\n", __FILE__,
			__LINE__, rc);
	return rc;
}

/*
 * msgque_cleanup 
 *
 * remove the message queue id from the system
 */
void	msgque_cleanup(int id) {
	/* remove the ipc structure					*/
	if (msgctl(id, IPC_RMID, NULL) < 0) {
		fprintf(stderr, "%s:%d: cannot remove the msg queue: %s (%d)\n",
			__FILE__, __LINE__, strerror(errno), errno);
	}
}

/*
 * msgque_sendquery
 *
 * pack a query into a message and send it to the message queue
 */
typedef struct querymsg {
	long	mtype;
	char	mtext[QUERYMSG_SIZE];
} querymsg_t;
int	msgque_sendquery(int id, const char *query, int size) {
	int		s, rc;
	querymsg_t	*qm;

	if (debug)
		fprintf(stderr, "%s:%d: sending query of length %d to queue\n",
			__FILE__, __LINE__, size);

	/* create a data structure for the message			*/
	s = sizeof(long) + size;
	qm = (querymsg_t *)malloc(sizeof(querymsg_t));
	time(&qm->mtype);

	/* copy the data to the message structure			*/
	memcpy(qm->mtext, query, size);
	if (debug)
		fprintf(stderr, "%s:%d: message packet of size %d ready\n",
			__FILE__, __LINE__, s);

	/* send the message to the queue				*/
	rc = msgsnd(id, qm, s, IPC_NOWAIT);
	if (rc < 0)
		fprintf(stderr, "%s:%d: sending message failed: %s (%d)\n",
			__FILE__, __LINE__, strerror(errno), errno);

	/* free the data allocated for the message			*/
	free(qm);
	return rc;
}

int	msgque_rcvquery(int id, char *query, int size) {
	querymsg_t	*qm;
	int		r;
	time_t		now;

	/* allocate a buffer large enough to hold the message		*/
	qm = (querymsg_t *)malloc(sizeof(querymsg_t));

	/* pick up a message from the queue				*/
	r = msgrcv(id, qm, QUERYMSG_SIZE, 0, 0);
	if (r < 0) {
		fprintf(stderr, "%s:%d: msg not retrieved: %s (%d)\n",
			__FILE__, __LINE__, strerror(errno), errno);
		return -1;
	}
	if (debug) {
		time(&now);
		fprintf(stderr, "%s:%d: processing message sent at %-24.24s, "
			"%d secs ago\n", __FILE__, __LINE__, ctime(&qm->mtype),
			now - qm->mtype);
	}

	/* check whether it fits in the query buffer, otherwise 	*/
	/* reallocate							*/
	if (r > size) {
		fprintf(stderr, "%s:%d: message buffer too small\n",
			__FILE__, __LINE__);
		return -1;
	}
	memcpy(query, qm->mtext, r);
	query[r] = '\0';
	free(qm);

	/* return the number of bytes					*/
	return r;
}
