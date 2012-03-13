/*
 * conftest.c -- check the syntax of a meteo graph configuration file
 *               and dump all info in the form of a configuration file
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: meteocheck.c,v 1.3 2001/12/26 22:10:46 afm Exp $
 */
#include <stdlib.h>
#include <stdio.h>
#include <meteo.h>
#include <mconf.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

extern int	optind;
extern char	*optarg;

char	*graphs[6] = { "pressure", "temperature", "wind", "rain",
		"solar-radiation", "uv-radiation" };
int	ngraphs = 6;
char	*sides[2] = { "left", "right" };
int	nsides = 2;
int	intervals[4] = { 300, 1800, 7200, 86400 };
int	nintervals = 4;
char	*prms[8] = { "min", "max", "scale", "start", "end", "step", "label",
		"color" };
int	nprms = 8;

int	main(int argc, char *argv[]) {
	int		c, i, j, k, l;
	FILE		*infile;
	char		*filename = NULL;
	mc_node_t	*mc;
	char		key[64];
	const md_t	*md;
	int		dump = 0, properties = 0;

	/* parse the command line					*/
	while (EOF != (c = getopt(argc, argv, "dFfPp")))
		switch (c) {
		case 'd':	/* increase debug level			*/
			debug++;
			break;
		case 'F':	/* suppress conf file output		*/
			dump = 0;
			break;
		case 'f':	/* display conf file on stdout (filter)	*/
			dump = 1;
			break;
		case 'P':	/* suppress properties 			*/
			properties = 0;
			break;
		case 'p':	/* display properties			*/
			properties = 1;
			break;
		default:
			break;
		}

	/* open the file specified in the remaining argument (take 	*/
	/* stdin if missing)						*/
	switch (argc - optind) {
	case 0:
		filename = "(stdin)";
		infile = stdin;
		break;
	case 1:
		filename = argv[optind];
		infile = fopen(argv[optind], "r");
		if (infile == NULL) {
			fprintf(stderr, "%s:%d: cannot open conf file '%s': "
				" %s (%d)\n", __FILE__, __LINE__,
				filename, strerror(errno), errno);
			exit(EXIT_FAILURE);
		}
		break;
	default:
		fprintf(stderr, "%s:%d: not capable of handling more than "
			"one file\n", __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}
	if (debug)
		fprintf(stderr, "%s:%d: configuration file %s opened\n",
			__FILE__, __LINE__, filename);

	/* read the configuration file					*/
	mc = mc_parse(infile);

	if (dump)
		mc_dump(stdout, mc);

	/* dump the configuration data to standard out			*/
	for (i = 0; i < ngraphs; i++)
	for (j = 0; j < nsides; j++)
	for (k = 0; k < nintervals; k++)
	for (l = 0; l < nprms; l++) {
		snprintf(key, sizeof(key), "%s.%s.%d.%s", graphs[i], sides[j],
			intervals[k], prms[l]);
		md = mc_getvalue(mc, key);
		if (md != NULL) {
			if (!properties) continue;
			switch (md->type) {
			case MC_STRING:
				printf("%s = \"%s\"\n", key, md->data.string);
				break;
			case MC_INTEGER:
				printf("%s = %d\n", key, md->data.ival);
				break;
			case MC_DOUBLE:
				printf("%s = %f\n", key, md->data.dval);
				break;
			case MC_COLOR:
				printf("%s = %d %d %d\n", key,
					md->data.color[0], md->data.color[1],
					md->data.color[2]);
				break;
			case MC_DATA:
				printf("%s = [configuration array]\n", key);
				break;
			}
		} else {
			fprintf(stderr, "%s missing\n", key);
		}
	}

	/* that's it							*/
	exit(EXIT_SUCCESS);
}
