/*
 * meteofixtimestamps.c -- fix the timestamps in a meteo database by producing a
 *                         a new stationdata table
 *
 * (c) 2002 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: meteofixtimestamps.c,v 1.1 2002/11/18 02:39:25 afm Exp $
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <mysql/mysql.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <string.h>
#include <assert.h>

int	verbose = 0;

#define	QUERYSIZE	10240

time_t	local2time(char *fromstring);
int	copy_rows(MYSQL *mysql, char *tablename, MYSQL_RES *res);
int	copy_table(MYSQL *mysql, char *tablename);
int	fix_timestamps_in(MYSQL *mysql, char *tablename);
void	usage(const char *progname);

/* usage message							*/
void	usage(const char *progname) {
	printf("usage: %s [ -v ] [ -h hostname ] [ -b database ] \\"
		"\t\t-u user -p password\n", progname);
	printf("\thostname defaults to `localhost', database to `meteo'\n");
}

/* convert a timestamp from localtime to GMT				*/
time_t	local2time(char *fromstring) {
	char		*wc;
	struct tm	tt;
	int		l;
	time_t		t;

	/* create a local working copy					*/
	l = strlen(fromstring);
	assert(l == 14);
	wc = (char *)alloca(l + 1);
	memcpy(wc, fromstring, l + 1);

	tt.tm_sec = 0; wc[12] = '\0';
	tt.tm_min = atoi(&wc[10]); wc[10] = '\0';
	tt.tm_hour = atoi(&wc[8]); wc[8] = '\0';
	tt.tm_mday = atoi(&wc[6]); wc[6] = '\0';
	tt.tm_mon = atoi(&wc[4]) - 1; wc[4] = '\0';
	tt.tm_year = atoi(wc) - 1900;
	tt.tm_yday = tt.tm_wday = tt.tm_isdst = 0;

	/* make time from that						*/
	t = mktime(&tt);
	return t;
}

/* copy_rows	copy all rows of a result to the new table, converting	*/
/*		timestamps to GMT as we go along			*/
int	copy_rows(MYSQL *mysql, char *tablename, MYSQL_RES *res) {
	char		query[QUERYSIZE];
	char		tostring[128];
	int		rows = 0, num_fields, i;
	MYSQL_ROW	row;
	MYSQL_FIELD	*f;
	struct tms	st, et;
	struct timeval	stv, etv;
	double		cputime, elapsedtime;

	/* remember when we started inserting				*/
	times(&st);
	gettimeofday(&stv, NULL);

	/* retrieve field info for target table				*/
	num_fields = mysql_num_fields(res);
	if (NULL == (f = mysql_fetch_fields(res))) {
		fprintf(stderr, "%s:%d: field information for '%sold' not "
			"available\n", __FILE__, __LINE__, tablename);
		return -1;
	}
	
	while ((row = mysql_fetch_row(res))) {
		/* create insert query					*/
		snprintf(query, sizeof(query), "insert into %s (", tablename);
		for (i = 0; i < num_fields; i++) {
			strcat(query, f[i].name);
			if (i < num_fields - 1)
				strcat(query, ", ");
		}
		strcat(query, ") values (");
		for (i = 0; i < num_fields; i++) {
			if (NULL == row[i]) {
				strcat(query, "NULL");
			} else {
				switch (f[i].type) {
				case FIELD_TYPE_TINY:
				case FIELD_TYPE_SHORT:
				case FIELD_TYPE_LONG:
				case FIELD_TYPE_INT24:
				case FIELD_TYPE_LONGLONG:
				case FIELD_TYPE_DECIMAL:
				case FIELD_TYPE_FLOAT:
				case FIELD_TYPE_DOUBLE:
				case FIELD_TYPE_YEAR:
				case FIELD_TYPE_DATE:
				case FIELD_TYPE_NEWDATE:
				case FIELD_TYPE_TIME:
				case FIELD_TYPE_DATETIME:
					strcat(query, row[i]);
					break;
				case FIELD_TYPE_TIMESTAMP:
					if (0 == strcmp(f[i].name, "timekey")) {
						snprintf(tostring,
							sizeof(tostring), "%d",
							(int)local2time(row[i]));
						strcat(query, tostring);
					} else {
						strcat(query, row[i]);
					}
					break;
				case FIELD_TYPE_STRING:
				case FIELD_TYPE_VAR_STRING:
					strcat(query, "'");
					mysql_real_escape_string(mysql,
						tostring, row[i],
						strlen(row[i]));
					strcat(query, tostring);
					strcat(query, "'");
					break;
				case FIELD_TYPE_BLOB:
				case FIELD_TYPE_TINY_BLOB:
				case FIELD_TYPE_MEDIUM_BLOB:
				case FIELD_TYPE_LONG_BLOB:
				case FIELD_TYPE_SET:
				case FIELD_TYPE_ENUM:
				case FIELD_TYPE_NULL:
					fprintf(stderr, "%s:%d: don't know how "
						"to handle field of BLOB/SET/"
						"ENUM/NULL type\n", __FILE__,
						__LINE__);
					return -1;
				}
			}
			if (i < num_fields - 1)
				strcat(query, ", ");
		}
		strcat(query, ")");
		if (verbose > 2)
			printf("%s:%d: update: %s\n", __FILE__, __LINE__,
				query);

		/* perform the query					*/
		if (mysql_query(mysql, query)) {
			fprintf(stderr, "%s:%d: problem updating the database: "
				"%s\n", __FILE__, __LINE__, mysql_error(mysql));
		} else {
			/* update the row counter			*/
			rows++;
		}
	}

	/* compute time it took to update all these rows		*/
	times(&et);
	cputime = ((et.tms_utime - st.tms_utime)
		+ (et.tms_stime - st.tms_stime))/100.;
	gettimeofday(&etv, NULL);
	elapsedtime = (etv.tv_sec + 1e-6 * etv.tv_usec) -
		(stv.tv_sec + 1e-6 * stv.tv_usec);
	if (verbose > 1)
		printf("%d rows, %.1f rows/sec, %.2fms CPU time per row\n",
			rows, rows/elapsedtime, 1000 * cputime/rows);

	return rows;
}

/* copy_table	copy the table from tablename to tablenamenew		*/
int	copy_table(MYSQL *mysql, char *tablename) {
	char		query[QUERYSIZE];
	MYSQL_RES	*res;
	MYSQL_ROW	row;
	time_t		starttime, endtime, t, finish;
	char		starttimestamp[32], endtimestamp[32], finishstamp[13];
	int		rows, totalrows = 0, tablerows;
	struct timeval	stv, etv;
	double		elapsedtime, remaining;

	gettimeofday(&stv, NULL);

	/* how large is this table?					*/
	snprintf(query, sizeof(query), "select count(*) from %sold", tablename);
	if (verbose)
		printf("%s:%d: find the number of rows to copy: %s\n",
			__FILE__, __LINE__, tablename);
	if (mysql_query(mysql, query)) {
		fprintf(stderr, "%s:%d: cannot find time limits for table "
			"'%s': %s\n", __FILE__, __LINE__, tablename,
			mysql_error(mysql));
		return -1;
	}
	res = mysql_store_result(mysql);
	if (NULL == res) {
		fprintf(stderr, "%s:%d: cannot store query result: %s\n",
			__FILE__, __LINE__, mysql_error(mysql));
		return -1;
	}
	row = mysql_fetch_row(res);
	tablerows = atoi(row[0]);
	mysql_free_result(res);
	printf("%d rows to update\n", tablerows);

	/* find the first and the last time stamp			*/
	snprintf(query, sizeof(query),
		"select unix_timestamp(min(timekey)), "
		"unix_timestamp(max(timekey)) from %sold", tablename);
	if (verbose)
		printf("%s:%d: finding time limits: %s\n", __FILE__, __LINE__,
			query);
	if (mysql_query(mysql, query)) {
		fprintf(stderr, "%s:%d: cannot find time limits for table "
			"'%s': %s\n", __FILE__, __LINE__, tablename,
			mysql_error(mysql));
		return -1;
	}
	res = mysql_store_result(mysql);
	if (NULL == res) {
		fprintf(stderr, "%s:%d: cannot store query result: %s\n",
			__FILE__, __LINE__, mysql_error(mysql));
		return -1;
	}
	row = mysql_fetch_row(res);
	starttime = atoi(row[0]);
	endtime = atoi(row[1]);
	mysql_free_result(res);
	if (verbose)
		printf("%s:%d: time limits: %d - %d\n", __FILE__, __LINE__,
			(int)starttime, (int)endtime);
	if ((starttime == 0) || (endtime == 0) || (endtime < starttime)) {
		fprintf(stderr, "%s:%d: inconsistent time stamps: %d - %d\n",
			__FILE__, __LINE__, (int)starttime, (int)endtime);
		return -1;
	}

	/* read through the table, one day at a time			*/
	for (t = starttime; t < endtime;) {
		/* find interval boundaries				*/
		strftime(starttimestamp, sizeof(starttimestamp),
			"%Y%m%d%H%M%S", localtime(&t));
		t += 86400;
		strftime(endtimestamp, sizeof(endtimestamp),
			"%Y%m%d%H%M%S", localtime(&t));

		/* read data from old table				*/
		snprintf(query, sizeof(query),
			"select * "
			"from %sold "
			"where timekey >= %s and timekey < %s", tablename,
			starttimestamp, endtimestamp);
		if (verbose > 1)
			printf("%s:%d: reading table '%sold': %s\n",
				__FILE__, __LINE__, tablename, query);
		if (mysql_query(mysql, query)) {
			fprintf(stderr, "%s:%d: cannot get data for interval "
				"[%s, %s): %s\n", __FILE__, __LINE__,
				starttimestamp, endtimestamp,
				mysql_error(mysql));
			return -1;
		}
		res = mysql_store_result(mysql);
		if (NULL == res) {
			fprintf(stderr, "%s:%d: cannot store retrieved data: "
				"%s\n", __FILE__, __LINE__, mysql_error(mysql));
			return -1;
		}
		if ((rows = copy_rows(mysql, tablename, res)) < 0) {
			fprintf(stderr, "%s:%d: cannot copy rows %s - %s\n",
				__FILE__, __LINE__,
				starttimestamp, endtimestamp);
			return -1;
		}
		mysql_free_result(res);
		totalrows += rows;

		/* log time needed so far, and estimate when finished	*/
		gettimeofday(&etv, NULL);
		elapsedtime = (etv.tv_sec + 1e-6 * etv.tv_usec) -
			(stv.tv_sec + 1e-6 * stv.tv_usec);
		remaining = (tablerows - totalrows) * (elapsedtime/totalrows);
		time(&finish);
		finish += remaining;
		strftime(finishstamp, sizeof(finishstamp),
			"%H:%M:%S", localtime(&finish));
		printf("%d rows in %.1fsec, %d remain (%.1fsec), "
			"complete at %s\n", totalrows, elapsedtime,
			tablerows - totalrows, remaining, finishstamp);
	}

	/* if we get to this point, the table has been read completely	*/
	return totalrows;
}

int	fix_timestamps_in(MYSQL *mysql, char *tablename) {
	int	totalrows;
	if (verbose)
		printf("%s:%d: fixing table '%s'\n", __FILE__, __LINE__,
			tablename);

	/* fill the new table from the old one				*/
	if ((totalrows = copy_table(mysql, tablename)) < 0) {
		fprintf(stderr, "%s:%d: copying data from %sold to %s failed\n",
			__FILE__, __LINE__, tablename, tablename);
		return -1;
	}
	printf("%d rows copied from '%sold' to '%s'\n", totalrows,
		tablename, tablename);

	if (verbose)
		printf("%s:%d: table '%s' updated\n", __FILE__, __LINE__,
			tablename);
	return 0;
}

int	main(int argc, char *argv[]) {
	char		*host = "localhost",
			*database = "meteo",
			*user = NULL,
			*password = NULL;
	MYSQL		mysql;
	int		c;

	/* parse command line						*/
	while (EOF != (c = getopt(argc, argv, "h:d:u:p:v?")))
		switch (c) {
		case '?':
			usage(argv[0]);
			exit(EXIT_SUCCESS);
			break;
		case 'h':
			host = optarg;
			break;
		case 'b':
			database = optarg;
			break;
		case 'u':
			user = optarg;
			break;
		case 'p':
			password = optarg;
			break;
		case 'v':
			verbose++;
			break;
		default:
			usage(argv[0]);
			exit(EXIT_FAILURE);
			break;
		}

	/* make sure we have all the data we need			*/
	if (host == NULL) {
		fprintf(stderr, "%s:%d: must specify host with -h option\n",
			__FILE__, __LINE__);
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}
	if (database == NULL) {
		fprintf(stderr, "%s:%d: must specify database with -b option\n",
			__FILE__, __LINE__);
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}
	if (user == NULL) {
		fprintf(stderr, "%s:%d: must specify user with -u option\n",
			__FILE__, __LINE__);
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}
	if (password == NULL) {
		fprintf(stderr, "%s:%d: must specify password with -p option\n",
			__FILE__, __LINE__);
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}

	/* connect to the database					*/
	mysql_init(&mysql);
	if (!mysql_real_connect(&mysql, host, user, password, database,
		0, NULL, 0)) {
		fprintf(stderr, "%s:%d: cannot connect to database: %s\n",
			__FILE__, __LINE__, mysql_error(&mysql));
		exit(EXIT_FAILURE);
	}

	/* fix stationdata and averages					*/
	fix_timestamps_in(&mysql, "stationdata");
	fix_timestamps_in(&mysql, "averages");

	/* inform user about success					*/
	printf("data has successfully been converted\n");

	/* cleanup							*/
	mysql_close(&mysql);
	exit(EXIT_SUCCESS);
}
