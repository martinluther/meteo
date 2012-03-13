/*
 * msgque.h -- handle message queues for asynchronous updates
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: msgque.h,v 1.3 2003/06/06 15:11:05 afm Exp $
 */
#ifndef _MSGQUE_H
#define _MSGQUE_H

#define MSGQUE_UNDEFINED	0
#define	MSGQUE_LISTEN		1
#define	MSGQUE_SPEAK		2

#ifdef __cplusplus
extern "C" {
#endif

typedef	struct {
	int	id;
	int	state;
	char	*filename;
} msgque_t;

extern msgque_t	*msgque_setup(const char *filename);
extern void	msgque_cleanup(msgque_t *id);
extern int	msgque_sendquery(msgque_t *queue, const char *query, int size);
extern int	msgque_rcvquery(msgque_t *queue, char *query, int size);

#ifdef __cplusplus
}
#endif

#endif /* _MSGQUE_H */
