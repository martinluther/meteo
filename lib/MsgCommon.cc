/*
 * MsgCommon.cc -- send messages to a queue
 * 
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: MsgCommon.cc,v 1.3 2004/02/25 23:48:05 afm Exp $
 */
#include <MsgCommon.h>

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
#include <MeteoException.h>

namespace meteo {

/*
 * msgque_setup
 *
 * create a message queue (if it does not exist yet)
 */
MsgCommon::MsgCommon(const std::string& name) : queuename(name) {
#ifdef HAVE_SYS_MSG_H
	key_t	mqkey;

	/* convert the queuename to a key				*/
	mqkey = ftok(queuename.c_str(), 1);
	if (mqkey == (key_t)-1) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, MDEBUG_ERRNO,
			"cannot convert key");
		throw MeteoException("cannot convert key", queuename);
	}

	/* get the msgqueue for the key					*/
	id = msgget(mqkey, IPC_CREAT | 0600);
	if (id < 0) {
		mdebug(LOG_ERR, MDEBUG_LOG, MDEBUG_ERRNO,
			"cannot get message queue");
		throw MeteoException("cannot get message queue", queuename);
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "msgque id %d allocated", id);
#else
	id = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (id < 0) {
		mdebug(LOG_ERR, MDEBUG_LOG, MDEBUG_ERRNO,
			"cannot get Unix domain socket");
		throw MeteoException("cannot get Unix domain socket", queuename);
	}
#endif
}

MsgCommon::~MsgCommon(void) {
#ifdef HAVE_SYS_MSG_H
	/* remove the ipc structure					*/
	if (msgctl(id, IPC_RMID, NULL) < 0) {
		mdebug(LOG_ERR, MDEBUG_LOG, MDEBUG_ERRNO,
			"cannot remove the msg queue");
	}
#else
	if (close(id) < 0) {
		mdebug(LOG_ERR, MDEBUG_LOG, MDEBUG_ERRNO,
			"cannot close Unix socket");
	}
#endif
}

} /* namespace meteo */
