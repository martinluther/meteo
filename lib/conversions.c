/*
 * conversions.c
 *
 * (c) 2001 unit conversions for meteorolical data
 *
 * $Id: conversions.c,v 1.2 2002/01/27 21:01:42 afm Exp $
 */
#include <conversions.h>
#include <stdlib.h>
#include <stdio.h>
#include <mdebug.h>

#define	NUNITS	12
static char	*unitnames[NUNITS] = {
	"",
	"degrees Fahrenheit",
	"degrees Celsius",
	"Kelvin",
	"mph",
	"km/h",
	"m/s",
	"hPa",
	"mm HG",
	"in HG",
	"mm",
	"in"
};

char	*unitname(int unit) {
	if ((unit < 0) || (unit > NUNITS)) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "illegal unit constant: %d",
			unit);
		return NULL;
	}
	return unitnames[unit];
}

#define	MILE	1.609344

double	unitconvert(int from, int to, double value) {
	switch (from) {
	case UNIT_NONE:
		return value;
		break;
	case UNIT_FAHRENHEIT:
		switch (to) {
		case UNIT_FAHRENHEIT:
			return value; break;
		case UNIT_CELSIUS:
			return 5. * (value - 32.) / 9.; break;
		case UNIT_KELVIN:
			return 273.15 + 5. * (value - 32.) / 9.; break;
		}
		break;
	case UNIT_CELSIUS:
		switch (to) {
		case UNIT_FAHRENHEIT:
			return 9. * value / 5. + 32.; break;
		case UNIT_CELSIUS:
			return value; break;
		case UNIT_KELVIN:
			return 273.15 + value; break;
		}
		break;
	case UNIT_KELVIN:
		switch (to) {
		case UNIT_FAHRENHEIT:
			return 9. * (value - 273.15)/5. +32;
		case UNIT_CELSIUS:
			return value - 273.15; break;
		case UNIT_KELVIN:
			return value; break;
		}
		break;
	case UNIT_MPH:
		switch (to) {
		case UNIT_MPH:
			return value; break;
		case UNIT_KMH:
			return MILE * value; break;
		case UNIT_MPS:
			return MILE * value/3.6; break;
		}
		break;
	case UNIT_KMH:
		switch (to) {
		case UNIT_MPH:
			return MILE * value; break;
		case UNIT_KMH:
			return value; break;
		case UNIT_MPS:
			return value / 3.6; break;
		}
		break;
	case UNIT_MPS:
		switch (to) {
		case UNIT_MPH:
			return 2.2369363 * value; break;
		case UNIT_KMH:
			return 3.6 * value; break;
		case UNIT_MPS:
			return value; break;
		}
		break;
	case UNIT_HPA:
		switch (to) {
		case UNIT_HPA:
			return value; break;
		case UNIT_MMHG:
			return value/1.333224; break;
		case UNIT_INHG:
			return 0.039370079 * value / 1.333224; break;
		}
		break;
	case UNIT_MMHG:
		switch (to) {
		case UNIT_HPA:
			return 1.333224 * value; break;
		case UNIT_MMHG:
			return value; break;
		case UNIT_INHG:
			return 0.039370079 * value; break;
		}
		break;
	case UNIT_INHG:
		switch (to) {
		case UNIT_HPA:
			return 33.8639 * value; break;
		case UNIT_MMHG:
			return 25.4 * value; break;
		case UNIT_INHG:
			return value; break;
		}
		break;
	case UNIT_MM:
		switch (to) {
		case UNIT_MM:
			return value; break;
		case UNIT_IN:
			return value / 25.4; break;
		}
		break;
	case UNIT_IN:
		switch (to) {
		case UNIT_MM:
			return value * 2.54; break;
		case UNIT_IN:
			return value; break;
		}
		break;
	case UNIT_UVINDEX:
		switch (to) {
		case UNIT_UVINDEX:
			return value; break;
		}
		break;
	case UNIT_WM2:
		switch (to) {
		case UNIT_WM2:
			return value; break;
		}
	}
	
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "incompatible units: %s -> %s",
		unitname(from), unitname(to));
	return 0.;
}
