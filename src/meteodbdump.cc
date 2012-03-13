/*
 * meteodbdump.cc -- dump the meteo database for a given set of stations
 *                   to suitable load files, and provide load stations
 *                   that will load the files again
 *
 *                   This program is written so that it can be compiled and
 *                   run independently of the meteo library. I use it to
 *                   synchronize the database on my notebook with timon's.
 *                   It isn't documented in a manual page either, because it
 *                   is hardly useful anybody but me.
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#include <stdio.h>
#include <stdlib.h>
#include <mysql.h>
#include <string>
#include <vector>
#include <errno.h>
#include <strings.h>

static int	averages = 0;
static int	stationdata = 0;
static int	debug = 0;

#define	N(a)	(NULL != a) ? a : "NULL"

static int	getchunk(MYSQL *mysql, const std::string& station, int timekey,
			FILE *stationdatafile, FILE *averagesfile,
			int chunksize) {
	if (debug)
		fprintf(stderr, "%s:%d: getchunk %d/%d\n", __FILE__, __LINE__,
			timekey, chunksize);
	char	query[1024];
	int	endtime;

	// perform a stationdata query for this station and at most chunksize
	// records
	snprintf(query, sizeof(query), 
		"select timekey, station, "
		"       year, month, mday, hour, min, "
		"	temperature, temperature_inside, barometer, "
		"	humidity, humidity_inside, rain, raintotal, "
		"	windspeed, winddir, windgust, windx, windy, duration, "
		"	solar, uv, samples, "
		"	group300, group1800, group7200, group86400 "
		"from stationdata "
		"where station = '%s' "
		"  and timekey > %d order by timekey limit %d",
		station.c_str(), timekey, chunksize);
	if (debug)
		fprintf(stderr, "%s:%d: stationdata query: %s\n",
			__FILE__, __LINE__, query);
	if (mysql_query(mysql, query)) {
		fprintf(stderr, "%s:%d: cannot perform query %s: %s\n",
			__FILE__, __LINE__, query, mysql_error(mysql));
		exit(EXIT_FAILURE);
	}
	if (debug)
		fprintf(stderr, "%s:%d: query complete\n", __FILE__, __LINE__);

	// read the data and write it to the stationdata file
	MYSQL_RES	*res = mysql_store_result(mysql);
	int	nrows = mysql_num_rows(res);
	if (debug)
		fprintf(stderr, "%s:%d: found %d rows in stationdat\n",
			__FILE__, __LINE__, nrows);
	stationdata += nrows;
	MYSQL_ROW	row;
	while (NULL != (row = mysql_fetch_row(res))) {
		endtime = atoi(row[0]);
		fprintf(stationdatafile,
			"insert into stationdata(timekey, station, "
			"	year, month, mday, hour, min, "
			"	temperature, temperature_inside, barometer, "
			"	humidity, humidity_inside, rain, raintotal, "
			"	windspeed, winddir, windgust, "
			"	windx, windy, duration, "
			"	solar, uv, samples, "
			"	group300, group1800, group7200, group86400) "
			"values (%s, '%s', "
			"	/* year */ %s, %s, %s, %s, %s, "
			"	/* temperature */ %s, %s, %s, "
			"	/* humidity */ %s, %s, %s, %s, "
			"	/* windspeed */ %s, %s, %s, "
			"	/* windx */ %s, %s, %s, "
			"	/* solar */ %s, %s, %s, "
			"	/* group300 */ %s, %s, %s, %s);\n",
				row[0], row[1],
				row[2], row[3], row[4], row[5], row[6], 
				N(row[7]), N(row[8]), N(row[9]),
				N(row[10]), N(row[11]), N(row[12]), N(row[13]),
				N(row[14]), N(row[15]), N(row[16]),
				N(row[17]), N(row[18]), N(row[19]),
				N(row[20]), N(row[21]), N(row[22]),
				N(row[23]), N(row[24]), N(row[25]), N(row[26])
			);
		stationdata++;
	}

	// free the result from this query
	mysql_free_result(res);

	// perform an averages query for this station and the interval starting
	// at timekey and ending at the timekey of the last record of the
	// previous query
	snprintf(query, sizeof(query),
		"select timekey, station, intval, "
		"	temperature, temperature_max, temperature_min, "
		"	temperature_inside, temperature_inside_max, "
		"		temperature_inside_min, "
		"	barometer, barometer_max, barometer_min, "
		"	humidity, humidity_max, humidity_min, "
		"	humidity_inside, humidity_inside_max, "
		"		humidity_inside_min, "
		"	rain, windspeed, winddir, windgust, solar, uv "
		"from averages "
		"where station = '%s' "
		"  and timekey > %d and timekey <= %d",
		station.c_str(), timekey, endtime);
	if (mysql_query(mysql, query)) {
		fprintf(stderr, "%s:%d: cannot perform query %s: %s\n",
			__FILE__, __LINE__, query, mysql_error(mysql));
		exit(EXIT_FAILURE);
	}

	// retrieve result for averages
	res = mysql_store_result(mysql);
	nrows = mysql_num_rows(res);
	if (debug)
		fprintf(stderr, "%s:%d: found %d rows in averages\n",
			__FILE__, __LINE__, nrows);
	averages += nrows;
	while ((row = mysql_fetch_row(res))) {
		fprintf(averagesfile,
			"insert into averages(timekey, station, intval, "
			"	temperature, temperature_max, temperature_min, "
			"	temperature_inside, temperature_inside_max, "
			"		temperature_inside_min, "
			"	barometer, barometer_max, barometer_min, "
			"	humidity, humidity_max, humidity_min, "
			"	humidity_inside, humidity_inside_max, "
			"		humidity_inside_min, "
			"	rain, windspeed, winddir, windgust, "
			"	solar, uv) values ("
			"	%s, '%s', %s, "
			"	/* temperature */ %s, %s, %s, "
			"	/* temperature_inside */ %s, %s, "
			"		%s, "
			"	/* barometer */ %s, %s, %s, "
			"	/* humidity */ %s, %s, %s, "
			"	/* humidity_inside */ %s, %s, "
			"		%s, "
			"	/* rain */ %s, %s, %s, %s, "
			"	/* solar */ %s, %s);\n",
			row[0], row[1], row[2],
			N(row[3]), N(row[4]), N(row[5]),
			N(row[6]), N(row[7]),
				N(row[8]),
			N(row[9]), N(row[10]), N(row[11]),
			N(row[12]), N(row[13]), N(row[14]),
			N(row[15]), N(row[16]),
				N(row[17]),
			N(row[18]), N(row[19]), N(row[20]), N(row[21]),
			N(row[22]), N(row[23])
		);
		averages++;
	}

	// free the result from this query
	mysql_free_result(res);

	// return the new last records timekey
	return endtime;
}


int	main(int argc, char *argv[]) {
	std::string	hostname("localhost");
	std::string	user("meteo");
	std::string	password("public");
	std::string	database("meteo");
	std::vector<std::string>	stations;
	std::string	basename("./");
	std::string	logurl("file:///-");
	FILE	*stationdatafile, *averagesfile;
	int	timekey = 0;
	int	chunksize = 1440; 	// one day worth of data
	MYSQL	mysql;
	int	c;

	// parse the command line
	while (EOF != (c = getopt(argc, argv, "db:h:l:u:p:s:f:c:")))
		switch (c) {
		case 'l':
			logurl = std::string(optarg);
			break;
		case 'd':
			debug++;
			break;
		case 'h':
			hostname = std::string(optarg);
			break;
		case 'u':
			user = std::string(optarg);
			break;
		case 'p':
			password = std::string(optarg);
			break;
		case 'b':
			database = std::string(optarg);
			break;
		case 's':
			// shorten the station name to 8 characters
			stations.push_back(std::string(optarg).substr(0, 8));
			break;
		case 'f':
			basename = std::string(optarg);
			break;
		case 'c':
			chunksize = atoi(optarg);
			break;
		}

	// connect to the database
	mysql_init(&mysql);
	if (NULL == mysql_real_connect(&mysql, hostname.c_str(), user.c_str(),
		password.c_str(), database.c_str(), 0, NULL, 0)) {
		fprintf(stderr, "%s:%d: cannot connect to database: %s\n",
			__FILE__, __LINE__, mysql_error(&mysql));
		exit(EXIT_FAILURE);
	}
	if (debug)
		fprintf(stderr, "%s:%d: connected to database\n",
			__FILE__, __LINE__);

	// open output files
	std::string	filename = basename + "stationdata";
	stationdatafile = fopen(filename.c_str(), "w");
	if (NULL == stationdatafile) {
		fprintf(stderr, "%s:%d: cannot open stationdata file %s: "
			"%s (%d)\n",
			__FILE__, __LINE__, filename.c_str(), strerror(errno),
			errno);
		exit(EXIT_FAILURE);
	}
	if (debug)
		fprintf(stderr, "%s:%d: stationdata file %s opened\n",
			__FILE__, __LINE__, filename.c_str());
	filename = basename + "averages";
	averagesfile = fopen(filename.c_str(), "w");
	if (NULL == averagesfile) {
		fprintf(stderr, "%s:%d: cannot averages open file %s: "
			"%s (%d)\n",
			__FILE__, __LINE__, filename.c_str(), strerror(errno),
			errno);
		exit(EXIT_FAILURE);
	}
	if (debug)
		fprintf(stderr, "%s:%d: averages file %s opened\n",
			__FILE__, __LINE__, filename.c_str());

	// read data from the database
	for (std::vector<std::string>::const_iterator i = stations.begin();
		i != stations.end(); i++) {
		if (debug)
			fprintf(stderr, "%s:%d: working on station %s\n",
				__FILE__, __LINE__, i->c_str());
		timekey = 0;
		do {
			int	oldtimekey = timekey;
			timekey = getchunk(&mysql, *i, timekey,
				stationdatafile, averagesfile, chunksize);
			if (timekey == oldtimekey)
				break;
		} while (timekey > 0);
	}

	// disconnect from database
	if (debug)
		fprintf(stderr, "%s:%d: disconnecting from database\n",
			__FILE__, __LINE__);
	mysql_close(&mysql);

	// close the files
	if (debug)
		fprintf(stderr, "%s:%d: closing files\n", __FILE__, __LINE__);
	fprintf(stationdatafile, "-- %d statements written\n", stationdata);
	fclose(stationdatafile);
	fprintf(averagesfile, "-- %d statements written\n", averages);
	fclose(averagesfile);

	// that's it
	exit(EXIT_SUCCESS);
}
