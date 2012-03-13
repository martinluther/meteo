/*
 * davis.h -- Davis Instruments weather station interface
 *
 * (c) 2001 Dr. Andreas Mueller
 *
 * $Id: davis.h,v 1.3 2001/12/26 22:10:45 afm Exp $
 */
#ifndef _DAVIS_H
#define _DAVIS_H

extern meteoaccess_t	*davis_new(meteocom_t *m);
extern int	vantage_wakeup(meteocom_t *m);

#endif /* _DAVIS_H */
