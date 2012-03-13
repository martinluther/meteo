/*
 * msgque.c -- handle message queues for asynchronous updates
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: msgque.c,v 1.2 2002/01/27 21:01:43 afm Exp $
 */
#include <stdlib.h>
#include <stdio.h>
#include <msgque.h>
#include <sys/msg.h>
#include <meteo.h>
#include <time.h>
#include <mdebug.h>

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
			mdebug(LOG_DEBUG, MDEBUG_LOG, MDEBUG_ERRNO,
				"cannot convert key");
		return -1;
	}

	/* get the msgqueue for the key					*/
	rc = msgget(mqkey, IPC_CREAT | 0600);
	if (rc < 0) {
		mdebug(LOG_ERR, MDEBUG_LOG, MDEBUG_ERRNO,
			"cannot get message queue");
		return -1;
	}
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "msgque id %d allocated", rc);
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
		mdebug(LOG_ERR, MDEBUG_LOG, MDEBUG_ERRNO,
			"cannot remove the msg queue");
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
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"sending query of length %d to queue", size);

	/* create a data structure for the message			*/
	s = sizeof(long) + size;
	qm = (querymsg_t *)malloc(sizeof(querymsg_t));
	time(&qm->mtype);

	/* copy the data to the message structure			*/
	memcpy(qm->mtext, query, size);
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"message packet of size %d ready", s);

	/* send the message to the queue				*/
	rc = msgsnd(id, qm, s, IPC_NOWAIT);
	if (rc < 0)
		mdebug(LOG_ERR, MDEBUG_LOG, MDEBUG_ERRNO,
			"sending message failed");

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
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "msg not retrieved");
		
		return -1;
	}
	if (debug) {
		time(&now);
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"processing message sent at %-24.24s, %d secs ago",
			ctime(&qm->mtype), now - qm->mtype);
	}

	/* check whether it fits in the query buffer, otherwise 	*/
	/* reallocate							*/
	if (r > size) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "message buffer too small");
		return -1;
	}
	memcpy(query, qm->mtext, r);
	query[r] = '\0';
	free(qm);

	/* return the number of bytes					*/
	return r;
}
