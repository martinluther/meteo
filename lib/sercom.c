/*
 * sercom.c -- serial communcations for davis instruments weather station
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sercom.h>
#include <string.h>
#include <meteo.h>
#include <mdebug.h>

meteocom_t	*sercom_new(meteoconf_t *mc, char *url, int speed) {
	meteocom_t	*r = NULL;
	struct termios	term;
	char		*dev;
	int		speedconst = B2400;

	/* make sure the dev specified is a file URL			*/
	if (strncmp(url, "file://", 7)) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "%s ist not a file url",
			  url);
		goto err;
	}
	dev = &url[7];
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "opening device %s", 
			 dev);
	
	/* allocate memory for the serial device file descriptor	*/
	r = com_new(mc);
	r->private = malloc(sizeof(int));
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "(int *)malloc(%d) = %p",
			  sizeof(int), r->private);

	/* open the serial device					*/
	if (0 >= (*(int *)r->private = open(dev, O_RDWR|O_NOCTTY))) {
		mdebug(LOG_ERR, MDEBUG_LOG, MDEBUG_ERRNO,
			"cannot open device");
		goto err;
	}
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "device '%s' open, fd = %d", 
			 dev, *(int *)r->private);

	/* set communication parameters	on the file descriptor		*/
	if (!isatty(*(int *)r->private)) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "device not a terminal");
		return NULL;
	}
	tcgetattr(*(int *)r->private, &term);

	/* set raw mode							*/
	term.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	term.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	term.c_cflag &= ~(CSIZE | PARENB);
	term.c_cflag |= CS8 | CREAD | CLOCAL;
	term.c_oflag &= ~(OPOST);
	term.c_cc[VMIN] = 1;
	term.c_cc[VTIME] = 0;

	/* determine the speed constant from the argument		*/
	switch (speed) {
		case 50:	speedconst = B50; break;
		case 75:	speedconst = B75; break;
		case 110:	speedconst = B110; break;
		case 134:	speedconst = B134; break;
		case 150:	speedconst = B150; break;
		case 200:	speedconst = B200; break;
		case 300:	speedconst = B300; break;
		case 600:	speedconst = B600; break;
		case 1200:	speedconst = B1200; break;
		case 2400:	speedconst = B2400; break;
		case 4800:	speedconst = B4800; break;
		case 9600:	speedconst = B9600; break;
		case 19200:	speedconst = B19200; break;
		case 38400:	speedconst = B38400; break;
		default:
			mdebug(LOG_ERR, MDEBUG_LOG, 0, "unknown speed: %d",
				  speed);
			return NULL;
	}
	cfsetispeed(&term, speedconst);
	cfsetospeed(&term, speedconst);
	if (tcsetattr(*(int *)r->private, TCSANOW, &term) < 0) {
		mdebug(LOG_ERR, MDEBUG_LOG, MDEBUG_ERRNO,
			"cannot set speed");
		goto err;
	}

	/* check the baud rate actually selected			*/
	tcgetattr(*(int *)r->private, &term);
	if (cfgetispeed(&term) != speedconst) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "ispeed not set correctly: %d",
			cfgetispeed(&term));
		goto err;
	}
	if (cfgetospeed(&term) != speedconst) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "ospeed not set correctly: %d",
			cfgetospeed(&term));
		goto err;
	}
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "speed set to %dn", 
			speed);

	/* don't need to set communication functions, defaults are ok	*/
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "serial port initialized");

	return r;

	/* error return							*/
err:
	return NULL;
}

void	sercom_free(meteocom_t *m) {
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "free((int *)%p)",
			  m->private);
	free(m->private);
	com_free(m);
}

