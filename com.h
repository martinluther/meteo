/*
 * com.h -- common data structures for communcation
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: com.h,v 1.4 2001/12/26 22:10:45 afm Exp $
 */
#ifndef _COM_H
#define _COM_H

#include <crc.h>
#include <sys/types.h>
#include <sys/time.h>

#define	ACK	6

#define	COM_VANTAGE 	1

typedef struct meteocom {
	int	(*get_char)(struct meteocom *);
	int	(*get_char_timed)(struct meteocom *, struct timeval *);
	int	(*put_char)(struct meteocom *, unsigned char);
	crc_t	crc;
	void	*private;
	int	flags;
} meteocom_t;

extern int	get_char(meteocom_t *m);
extern int	get_char_timed(meteocom_t *m, struct timeval *timeout);
extern int	put_char(meteocom_t *m, unsigned char c);
extern int	put_string(meteocom_t *m, char *);
extern int	put_unsigned(meteocom_t *m, unsigned short s);
extern int	get_acknowledge(meteocom_t *m);
extern int	get_acknowledge_timed(meteocom_t *m, struct timeval *timeout);
extern int	get_buffer(meteocom_t *m, unsigned char *b, int n);

extern meteocom_t	*com_new(void);
extern void		com_free(meteocom_t *);

#endif /* _COM_H */
