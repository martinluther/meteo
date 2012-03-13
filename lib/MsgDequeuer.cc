/*
 * msgque.c -- handle message queues for asynchronous updates
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: MsgDequeuer.cc,v 1.2 2003/10/14 23:47:45 afm Exp $
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
#include <time.h>
#include <mdebug.h>
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#include <MsgDequeuer.h>
#include <MeteoException.h>

#define QUERYMSG_SIZE	4096

namespace meteo {

MsgDequeuer::MsgDequeuer(const std::string& name) : MsgCommon(name) {
#ifndef HAVE_SYS_MSG_H
	/* form the socket address				*/
	struct sockaddr_un	sau;
	sau.sun_family = AF_UNIX;
	strncpy(sau.sun_path, queuename.c_str(), queuename.size() + 1);
	sau.sun_len = queuename.size() + 1;

	/* bind the socket to the file				*/
	if (bind(id, (struct sockaddr *)&sau, sizeof(sau)) < 0) {
		mdebug(LOG_ERR, MDEBUG_LOG, MDEBUG_ERRNO,
			"connecting socket failed");
		throw MeteoException("connecting socket failed", "");
	}
#endif /* HAVE_SYS_MSG_H */
}

MsgDequeuer::~MsgDequeuer(void) {
#ifndef HAVE_SYS_MSG_H
	unlink(queuename.c_str());
#endif
}

typedef struct querymsg {
	long	mtype;
	char	mtext[QUERYMSG_SIZE];
} querymsg_t;

std::string	MsgDequeuer::operator()(void) {
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
	r = msgrcv(id, qm, QUERYMSG_SIZE, 0, 0);
#else
	r = recv(id, qm, QUERYMSG_SIZE, 0);
#endif
	if (r < 0) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "msg not retrieved");
		throw MeteoException("message not received", "");
	}
	if (debug) {
		time(&now);
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"processing message sent at %-24.24s, %d secs ago",
			ctime(&qm->mtype), now - qm->mtype);
	}

	/* check whether it fits in the query buffer, otherwise 	*/
	/* reallocate							*/
	std::string	result(qm->mtext, r - sizeof(qm->mtype));
	if (NULL != qm)
		free(qm);

	/* return the number of bytes					*/
	return result;
}

} /* namespace meteo */
