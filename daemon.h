/*
 * daemon.h -- common functions that help in becoming a daemon
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: daemon.h,v 1.1 2001/12/29 20:39:54 afm Exp $
 */
#ifndef _DAEMON_H
#define _DAEMON_H

extern int	daemonize(const char *pidfilenamepattern, const char *station);

#endif /* _DAEMON_H */
