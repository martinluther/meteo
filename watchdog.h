/*
 * watchdog.h -- watchdoc functionality for the meteo package
 *
 * (c) 2002 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id$
 */
#ifndef _WATCHDOG_H
#define _WATCHDOG_H

extern int	wd_setup(int argc, char *argv[]);
extern int	wd_arm(int timeout);
extern int	wd_disarm(void);
extern void	wd_fire(const char *reason);

#endif /* _WATCHDOG_H */
