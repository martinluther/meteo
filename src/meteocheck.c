/*
 * meteocheck.c -- check the syntax of a meteo graph configuration file
 *                 and dump all info in the form of a configuration file
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: meteocheck.c,v 1.2 2002/01/27 21:01:44 afm Exp $
 */
#include <stdlib.h>
#include <stdio.h>
#include <meteo.h>
#include <mconf.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <printver.h>
#include <mdebug.h>

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
	while (EOF != (c = getopt(argc, argv, "dFfPpV")))
		switch (c) {
		case 'l':
			if (mdebug_setup_file("meteocheck", stderr) < 0) {
				fprintf(stderr, "%s: cannot init log\n",
					argv[0]);
				exit(EXIT_FAILURE);
			}
			break;
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
		case 'V':
			fprintver(stdout);
			exit(EXIT_SUCCESS);
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
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
				"cannot open conf file '%s'", filename);
			exit(EXIT_FAILURE);
		}
		break;
	default:
		mdebug(LOG_CRIT, MDEBUG_LOG, 0,
			"not capable of handling more than one file");
		exit(EXIT_FAILURE);
	}
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"configuration file %s opened", filename);

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
			mdebug(LOG_ERR, MDEBUG_LOG, 0, "%s missing", key);
		}
	}

	/* that's it							*/
	exit(EXIT_SUCCESS);
}
