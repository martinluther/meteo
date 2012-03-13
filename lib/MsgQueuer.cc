/*
 * MsgQueuer.cc -- send messages to a queue
 * 
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: MsgQueuer.cc,v 1.6 2009/01/10 19:00:24 afm Exp $
 */
#include <MsgQueuer.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif /* HAVE_STDLIB_H */
#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif /* HAVE_STDIO_H */
#ifdef HAVE_SYS_MSG_H
#include <sys/msg.h>
#endif /* HAVE_SYS_MSG_H */
#ifdef HAVE_SYS_IPC_H
#include <sys/ipc.h>
#endif /* HAVE_SYS_IPC_H */
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif /* HAVE_SYS_TYPES_H */
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif /* HAVE_SYS_SOCKET_H */
#ifdef HAVE_SYS_UN_H
#include <sys/un.h>
#endif /* HAVE_SYS_UN_H */
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */
#ifdef HAVE_TIME_H
#include <time.h>
#endif /* HAVE_TIME_H */
#include <mdebug.h>
#ifdef HAVE_STRING_H
#include <string.h>
#endif /* HAVE_STRING_H */
#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#endif /* HAVE_ALLOCA_H */

#define QUERYMSG_SIZE	4096

namespace meteo {

/*
 * msgque_setup
 *
 * create a message queue (if it does not exist yet)
 */
MsgQueuer::MsgQueuer(const std::string& name) : MsgCommon(name) {
}

MsgQueuer::~MsgQueuer(void) {
}

typedef struct querymsg {
	long	mtype;
	char	mtext[QUERYMSG_SIZE];
} querymsg_t;

void	MsgQueuer::operator()(const std::string& query) {
	int		s;
	querymsg_t	*qm = NULL;

	int	size = query.size();
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
		"sending query of length %d to queue", size);

	// create a data structure for the message
	s = sizeof(long) + size;
	qm = (querymsg_t *)alloca(sizeof(querymsg_t));
	time(&qm->mtype);

	// copy the data to the message structure
	memcpy(qm->mtext, query.c_str(), size);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
		"message packet of size %d ready", s);

#ifdef HAVE_SYS_MSG_H
	// send the message to the queue
	if (msgsnd(id, qm, s, IPC_NOWAIT) < 0)
		mdebug(LOG_ERR, MDEBUG_LOG, MDEBUG_ERRNO,
			"sending message failed");
#else
	// form the socket address
	struct sockaddr_un	sau;
	sau.sun_family = AF_UNIX;
	strncpy(sau.sun_path, queuename.c_str(), sizeof(sau.sun_path));
	sau.sun_len = queuename.size() + 1;

	if (s != sendto(id, qm, s, 0, (struct sockaddr *)&sau,
		sizeof(sau))) {
		mdebug(LOG_ERR, MDEBUG_LOG, MDEBUG_ERRNO,
			"could not send to socket");
	}
#endif
}

} /* namespace meteo */
