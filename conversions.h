/*
 * conversions.h
 *
 * (c) 2001 unit conversions for meteorolical data
 *
 * $Id: conversions.h,v 1.3 2001/12/26 22:10:45 afm Exp $
 */
#ifndef _CONVERSIONS_H
#define _CONVERSIONS_H

#define UNIT_NONE	0

#define	UNIT_FAHRENHEIT	1
#define	UNIT_CELSIUS	2
#define	UNIT_KELVIN	3

#define UNIT_MPH	4
#define	UNIT_KMH	5
#define	UNIT_MPS	6

#define	UNIT_HPA	7
#define	UNIT_MMHG	8
#define	UNIT_INHG	9

#define UNIT_MM		10
#define	UNIT_IN		11

#define UNIT_UVINDEX	12
#define UNIT_WM2	13

extern double	unitconvert(int from, int to, double value);
extern char	*unitname(int unit);

#endif /* _CONVERSIONS_H */
