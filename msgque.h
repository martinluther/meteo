/*
 * msgque.h -- handle message queues for asynchronous updates
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: msgque.h,v 1.2 2001/12/26 22:10:46 afm Exp $
 */
#ifndef _MSGQUE_H
#define _MSGQUE_H

extern int	msgque_setup(const char *filename);
extern void	msgque_cleanup(int id);
extern int	msgque_sendquery(int queue, const char *query, int size);
extern int	msgque_rcvquery(int queue, char **query, int size);

#endif /* _MSGQUE_H */
