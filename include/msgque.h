/*
 * msgque.h -- handle message queues for asynchronous updates
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: msgque.h,v 1.1 2002/01/18 23:34:27 afm Exp $
 */
#ifndef _MSGQUE_H
#define _MSGQUE_H

extern int	msgque_setup(const char *filename);
extern void	msgque_cleanup(int id);
extern int	msgque_sendquery(int queue, const char *query, int size);
extern int	msgque_rcvquery(int queue, char *query, int size);

#endif /* _MSGQUE_H */
