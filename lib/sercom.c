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
#include <errno.h>
#include <meteo.h>

meteocom_t	*sercom_new(char *url, int speed) {
	meteocom_t	*r = NULL;
	struct termios	term;
	char		*dev;
	int		speedconst = B2400;

	/* make sure the dev specified is a file URL			*/
	if (strncmp(url, "file://", 7)) {
		fprintf(stderr, "%s:%d: %s ist not a file url\n",
			__FILE__, __LINE__, url);
		goto err;
	}
	dev = &url[7];
	if (debug)
		fprintf(stderr, "%s:%d: opening device %s\n", __FILE__,
			__LINE__, dev);
	
	/* allocate memory for the serial device file descriptor	*/
	r = com_new();
	r->private = malloc(sizeof(int));
	if (debug)
		fprintf(stderr, "%s:%d: (int *)malloc(%d) = %p\n",
			__FILE__, __LINE__, sizeof(int), r->private);

	/* open the serial device					*/
	if (0 >= (*(int *)r->private = open(dev, O_RDWR|O_NOCTTY))) {
		fprintf(stderr, "%s:%d: cannot open device: %s (%d)\n",
			__FILE__, __LINE__, strerror(errno), errno);
		goto err;
	}
	if (debug)
		fprintf(stderr, "%s:%d: device '%s' open, fd = %d\n", __FILE__,
			__LINE__, dev, *(int *)r->private);

	/* set communication parameters	on the file descriptor		*/
	if (!isatty(*(int *)r->private)) {
		fprintf(stderr, "%s:%d: device not a terminal\n",
			__FILE__, __LINE__);
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
			fprintf(stderr, "%s:%d: unknown speed: %d\n",
				__FILE__, __LINE__, speed);
			return NULL;
	}
	cfsetispeed(&term, speedconst);
	cfsetospeed(&term, speedconst);
	if (tcsetattr(*(int *)r->private, TCSANOW, &term) < 0) {
		fprintf(stderr, "%s:%d: cannot set speed: %s (%d)\n", __FILE__,
			__LINE__, strerror(errno), errno);
		goto err;
	}

	/* check the baud rate actually selected			*/
	tcgetattr(*(int *)r->private, &term);
	if (cfgetispeed(&term) != speedconst) {
		fprintf(stderr, "%s:%d: ispeed not set correctly: %d\n",
			__FILE__, __LINE__, cfgetispeed(&term));
		goto err;
	}
	if (cfgetospeed(&term) != speedconst) {
		fprintf(stderr, "%s:%d: ospeed not set correctly: %d\n",
			__FILE__, __LINE__, cfgetospeed(&term));
		goto err;
	}
	if (debug)
		fprintf(stderr, "%s:%d: speed set to %d\n", __FILE__,
			__LINE__, speed);

	/* don't need to set communication functions, defaults are ok	*/
	if (debug)
		fprintf(stderr, "%s:%d: serial port initialized\n",
			__FILE__, __LINE__);

	return r;

	/* error return							*/
err:
	return NULL;
}

void	sercom_free(meteocom_t *m) {
	if (debug)
		fprintf(stderr, "%s:%d: free((int *)%p)\n",
			__FILE__, __LINE__, m->private);
	free(m->private);
	com_free(m);
}

