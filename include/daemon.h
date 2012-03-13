/*
 * daemon.h -- common functions that help in becoming a daemon
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: daemon.h,v 1.2 2003/06/06 15:11:05 afm Exp $
 */
#ifndef _DAEMON_H
#define _DAEMON_H

#ifdef __cplusplus
extern "C" {
#endif

extern int	daemonize(const char *pidfilenamepattern, const char *station);

#ifdef __cplusplus
}
#endif

#endif /* _DAEMON_H */
