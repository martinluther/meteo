/*
 * dloop.c -- davis loop processing
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: dloop.c,v 1.1 2002/01/18 23:34:29 afm Exp $
 */
#include <config.h>
#include <dloop.h>
#include <stdlib.h>
#include <stdio.h>
#include <meteo.h>
#include <string.h>

static void	imagedump(unsigned char *image, int length) {
	char	hex[50], text[20];
	int	i;
	for (i = 0; i < length; i++) {
		if (i % 16 == 0) {
			memset(hex, 0, sizeof(hex));
			memset(text, 0, sizeof(text));
		}
		sprintf(hex + strlen(hex), "%02x", image[i]);
		sprintf(text + strlen(text), "%c",
			((image[i] >= ' ') && (image[i] < 127))
				? image[i] : '.');
		if (i % 16 == 15) {
			printf("%02d  %-40.40s %-17.17s\n", 16 * (i / 16),
				hex, text);
		}
		if (i % 4 == 3)
			strcat(hex, " ");
		if (i % 8 == 7) {
			strcat(hex, " ");
			strcat(text, " ");
		}
	}
}

#define	MONITOR_LOOP_SIZE	17
static int	get_monitor_image(loop_t *l, meteodata_t *m) {
	unsigned char	image[32];
	unsigned char	a;
	double		temperature, temperature_inside, humidity,
			humidity_inside, barometer, speed, direction,
			rain;

	if (debug)
		fprintf(stderr, "%s:%d: start reading sensor image\n",
			__FILE__, __LINE__);
	/* wait for a 01 character					*/
	a = get_char(l->m->m);
	if (a != 0x01) {
		fprintf(stderr, "%s:%d: start byte not received\n",	
			__FILE__, __LINE__);
		return -1;
	}

	/* read 17 bytes						*/
	if (get_buffer(l->m->m, image, MONITOR_LOOP_SIZE)) {
		fprintf(stderr, "%s:%d: problem reading block\n",
			__FILE__, __LINE__);
		return -1;
	}
	if (debug > 1)
		imagedump(image, MONITOR_LOOP_SIZE);

	/* convert 17 bytes to data					*/
	temperature_inside = (256. * image[1] + image[0])/10.;
	temperature = (256. * image[3] + image[2])/10.;
	speed = unitconvert(UNIT_MPH, UNIT_MPS, (double)image[4]);
	direction = 256. * image[6] + image[5];
	barometer = (256. * image[8] + image[7])/1000.;
	humidity_inside = (double)image[9];
	humidity = (double)image[10];
	rain = (double)image[11]/5.;

	/* update the meteodata record					*/
	meteodata_update(m, temperature, temperature_inside,
		humidity, humidity_inside, barometer, speed, direction, rain,
		0., 0.);

	/* return the filled in sensor image				*/
	return 0;
}

#define	VANTAGE_LOOP_SIZE	99
static int	get_vantage_image(loop_t *l, meteodata_t *m) {
	unsigned char	image[VANTAGE_LOOP_SIZE];
	double		temperature, temperature_inside, humidity,
			humidity_inside, barometer, speed, direction,
			rain, uv, solar;
	int		i;

	if (debug)
		fprintf(stderr, "%s:%d: about to read a LOOP packet of "
			"size %d\n", __FILE__, __LINE__, VANTAGE_LOOP_SIZE);

	/* read a complete packet from the link				*/
	for (i = 0; i < VANTAGE_LOOP_SIZE; i++) {
		if ((debug > 0) && ((i % 10) == 0))
			fprintf(stderr, "%s:%d: received %d characters so "
				"far\n", __FILE__, __LINE__, i);
		image[i] = get_char(l->m->m);
	}
	if (debug)
		fprintf(stderr, "%s:%d: got a packet\n", __FILE__, __LINE__);
	if (debug > 1)
		imagedump(image, VANTAGE_LOOP_SIZE);

	/* check for loop packet type					*/
	if (0 != strcmp("LOOP", (char *)image)) {
		fprintf(stderr, "%s:%d: this is not a Vantage LOOP packet\n",
			__FILE__, __LINE__);
		return -1;
	}

	/* convert data to double values				*/
	barometer = (image[8] * 256. + image[7])/1000.;
	temperature_inside = (image[10] * 256. + image[9])/10.;
	humidity_inside = (double)image[11];
	temperature = (image[13] * 256. + image[12])/10.;
	speed = unitconvert(UNIT_MPH, UNIT_MPS, (double)image[14]);
	if (debug)
		fprintf(stderr, "%s:%d: speed value: %.1f m/s\n",
			__FILE__, __LINE__, speed);
	direction = image[17] * 256. + image[16];
	humidity = (double)image[33];
	rain = unitconvert(UNIT_IN, UNIT_MM,
		(image[51] * 256. + image[50])/10.);
	uv = (double)image[43]/10.;
	solar = (image[45] * 256. + image[44]);

	/* update the meteodata record					*/
	meteodata_update(m, temperature, temperature_inside,
		humidity, humidity_inside, barometer, speed, direction, rain,
		solar, uv);

	/* return the filled in sensor image				*/
	return 0;
}

loop_t	*dloop_new(meteoaccess_t *m) {
	loop_t	*l;
	l = (loop_t *)malloc(sizeof(loop_t));
	if (debug)
		fprintf(stderr, "%s:%d: (loop_t *)malloc(%d) = %p\n",
			__FILE__, __LINE__, sizeof(loop_t), l);
	l->m = m;
	if (m->m->flags & COM_VANTAGE)
		l->read = get_vantage_image;
	else
		l->read = get_monitor_image;
	if (debug)
		fprintf(stderr, "%s:%d: read function at %p\n", __FILE__,
			__LINE__, l->read);
	return l;
}
