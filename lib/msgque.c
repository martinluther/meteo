/*
 * msgque.c -- handle message queues for asynchronous updates
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: msgque.c,v 1.8 2003/06/12 23:29:46 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif
#include <msgque.h>
#ifdef HAVE_SYS_MSG_H
#include <sys/msg.h>
#endif /* HAVE_SYS_MSG_H */
#ifdef HAVE_SYS_IPC_H
#include <sys/ipc.h>
#endif /* HAVE_SYS_IPC_H */
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#include <sys/un.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <meteo.h>
#include <time.h>
#include <mdebug.h>
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#define QUERYMSG_SIZE	4096

/*
 * msgque_setup
 *
 * create a message queue (if it does not exist yet)
 */
msgque_t	*msgque_setup(const char *filename) {
	msgque_t	*rc = NULL;
	int		mid;
#ifdef HAVE_SYS_MSG_H
	key_t	mqkey;

	/* convert the filename to a key				*/
	mqkey = ftok(filename, 1);
	if (mqkey == (key_t)-1) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, MDEBUG_ERRNO,
			"cannot convert key");
		return rc;
	}

	/* get the msgqueue for the key					*/
	mid = msgget(mqkey, IPC_CREAT | 0600);
	if (mid < 0) {
		mdebug(LOG_ERR, MDEBUG_LOG, MDEBUG_ERRNO,
			"cannot get message queue");
		return rc;
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "msgque id %d allocated", rc);
#else
	mid = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (mid < 0) {
		mdebug(LOG_ERR, MDEBUG_LOG, MDEBUG_ERRNO,
			"cannot get Unix domain socket");
		return rc;
	}
#endif
	/* initialize a structure containing the message queue or	*/
	/* socket id							*/
	rc = (msgque_t *)malloc(sizeof(msgque_t));
	rc->state = MSGQUE_UNDEFINED;
	rc->filename = strdup(filename);
	rc->id = mid;

	/* return the complete structure				*/
	return rc;
}

/*
 * msgque_cleanup 
 *
 * remove the message queue id from the system
 */
void	msgque_cleanup(msgque_t *q) {
#ifdef HAVE_SYS_MSG_H
	/* remove the ipc structure					*/
	if (msgctl(q->id, IPC_RMID, NULL) < 0) {
		mdebug(LOG_ERR, MDEBUG_LOG, MDEBUG_ERRNO,
			"cannot remove the msg queue");
	}
#else
	if (close(q->id) < 0) {
		mdebug(LOG_ERR, MDEBUG_LOG, MDEBUG_ERRNO,
			"cannot close Unix socket");
	}
	if (MSGQUE_LISTEN == q->state) {
		unlink(q->filename);
	}
#endif
	if (NULL != q->filename) {
		free(q->filename);
		q->filename = NULL;
	}
	free(q);
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
int	msgque_sendquery(msgque_t *id, const char *query, int size) {
	int		s, rc = -1;
	querymsg_t	*qm = NULL;
#ifndef HAVE_SYS_MSG_H
	struct sockaddr_un	sau;
#endif

	mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
		"sending query of length %d to queue", size);

	/* create a data structure for the message			*/
	s = sizeof(long) + size;
	qm = (querymsg_t *)malloc(sizeof(querymsg_t));
	time(&qm->mtype);

	/* copy the data to the message structure			*/
	memcpy(qm->mtext, query, size);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
		"message packet of size %d ready", s);

#ifdef HAVE_SYS_MSG_H
	/* send the message to the queue				*/
	rc = msgsnd(id->id, qm, s, IPC_NOWAIT);
	if (rc < 0)
		mdebug(LOG_ERR, MDEBUG_LOG, MDEBUG_ERRNO,
			"sending message failed");
#else
	/* first we must check whether we are already connected		*/
	if (MSGQUE_LISTEN == id->state) {
		mdebug(LOG_ERR, MDEBUG_LOG, MDEBUG_ERRNO,
			"sending impossible: socket inconsistent");
		goto cleanup;
	}
	id->state = MSGQUE_SPEAK;

	/* form the socket address				*/
	sau.sun_family = AF_UNIX;
	strncpy(sau.sun_path, id->filename, sizeof(sau.sun_path));
	sau.sun_len = strlen(id->filename) + 1;

	if (s != sendto(id->id, qm, s, 0, (struct sockaddr *)&sau,
		sizeof(sau))) {
		mdebug(LOG_ERR, MDEBUG_LOG, MDEBUG_ERRNO,
			"could not send to socket");
	}

	/* free the data allocated for the message			*/
cleanup:
#endif
	free(qm);
	return rc;
}

int	msgque_rcvquery(msgque_t *id, char *query, int size) {
	querymsg_t	*qm = NULL;
	int		r = -1;
	time_t		now;
#ifndef HAVE_SYS_MSG_H
	struct sockaddr_un	sau;
#endif

	/* allocate a buffer large enough to hold the message		*/
	qm = (querymsg_t *)malloc(sizeof(querymsg_t));

	/* pick up a message from the queue				*/
#ifdef HAVE_SYS_MSG_H
	r = msgrcv(id->id, qm, QUERYMSG_SIZE, 0, 0);
#else
	if (MSGQUE_SPEAK == id->state) {
		mdebug(LOG_ERR, MDEBUG_LOG, MDEBUG_ERRNO,
			"receiving impossible: socket inconsistent");
		goto cleanup;
	}
	if (MSGQUE_LISTEN != id->state) {
		/* form the socket address				*/
		sau.sun_family = AF_UNIX;
		strncpy(sau.sun_path, id->filename, sizeof(sau.sun_path));
		sau.sun_len = strlen(id->filename) + 1;

		/* bind the socket to the file				*/
		if (bind(id->id, (struct sockaddr *)&sau, sizeof(sau)) < 0) {
			mdebug(LOG_ERR, MDEBUG_LOG, MDEBUG_ERRNO,
				"connecting socket failed");
			goto cleanup;
		}
		id->state = MSGQUE_LISTEN;
	}
	r = recv(id->id, qm, QUERYMSG_SIZE, 0);
#endif
	if (r < 0) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "msg not retrieved");
		goto cleanup;
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
		goto cleanup;
	}
	memcpy(query, qm->mtext, r);
	query[r] = '\0';
cleanup:
	if (NULL != qm)
		free(qm);

	/* return the number of bytes					*/
	return r;
}
