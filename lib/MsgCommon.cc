/*
 * MsgCommon.cc -- send messages to a queue
 * 
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: MsgCommon.cc,v 1.4 2009/01/10 19:00:24 afm Exp $
 */
#include <MsgCommon.h>

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
#include <MeteoException.h> /* HAVE_STRING_H */

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
		mdebug(LOG_ERR, MDEBUG_LOG, MDEBUG_ERRNO,
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
