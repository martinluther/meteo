/*
 * meteodbmigrate.cc -- read the database using the same sophisticated technique
 *                      as in meteodbdump and write the information to the
 *                      new database structure. Do not modify the existing
 *                      tables in any way
 *
 * In most cases, the fields can just be copied over from the old database
 * to the new one. However, for wind, there is a change in interpretation
 * of the fields. In pre 0.9.0 versions, the windx and windy fields meant
 * x and y component of wind. In newer versions, the mean x and y components
 * of the vector describing the displacement during the measurement interval.
 * Therefore, when migrating the wind data from the sdata table, we have
 * to multiply by the duration.
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#include <stdio.h>
#include <stdlib.h>
#include <mysql.h>
#include <string>
#include <vector>
#include <map>
#include <errno.h>
#include <strings.h>
#include <unistd.h>

static int	averages = 0;
static int	stationdata = 0;
static int	debug = 0;
static int	offset = 0;
static bool	quiet = false;

struct fieldmap_s {
	char	*fieldname;
	char	*sensorname;
	char	*mfieldname;
	int	instationdata;
	int	inaverages;
};
typedef struct fieldmap_s fieldmap_t;

#ifdef LAJEADO
#define	VANTAGE_NFIELDS	28
#else
#define	VANTAGE_NFIELDS 26
#endif
fieldmap_t	vantagemap[VANTAGE_NFIELDS] = {
  /* fieldname */               /* sensor */    /* mfieldname */
{ "temperature",		"iss",		"temperature",		1, 1 },
{ "temperature_min",		"iss",		"temperature_min",	0, 1 },
{ "temperature_max",		"iss",		"temperature_max",	0, 1 },
{ "temperature_inside",		"console",	"temperature",		1, 1 },
{ "temperature_inside_min",	"console",	"temperature_min",	0, 1 },
{ "temperature_inside_max",	"console",	"temperature_max",	0, 1 },
{ "humidity",			"iss",		"humidity",		1, 1 },
{ "humidity_min",		"iss",		"humidity_min",		0, 1 },
{ "humidity_max",		"iss",		"humidity_max",		0, 1 },
{ "humidity_inside",		"console",	"humidity",		1, 1 },
{ "humidity_inside_min",	"console",	"humidity_min",		0, 1 },
{ "humidity_inside_max",	"console",	"humidity_max",		0, 1 },
{ "barometer",			"console",	"barometer",		1, 1 },
{ "barometer_min",		"console",	"barometer_min",	0, 1 },
{ "barometer_max",		"console",	"barometer_max",	0, 1 },
{ "solar",			"iss",		"solar",		1, 1 },
{ "uv",				"iss",		"uv",			1, 1 },
{ "rain",			"iss",		"rain",			1, 1 },
{ "raintotal",			"iss",		"raintotal",		1, 0 },
{ "windspeed",			"iss",		"wind",			1, 1 },
{ "winddir",			"iss",		"winddir",		1, 1 },
{ "windgust",			"iss",		"windgust",		1, 1 },
{ "windx",			"iss",		"windx",		1, 0 },
{ "windy",			"iss",		"windy",		1, 0 },
#ifdef LAJEADO
{ "soilmoisture1",		"soil1",	"moisture",		1, 1 },
{ "temperature_soil1",		"soil1",	"temperature",		1, 1 },
#endif
{ "duration",			"iss",	"duration",			1, 0 },
{ "samples",			"iss",	"samples",			1, 0 }
};

#define	WMII_NFIELDS	24
fieldmap_t	wmiimap[WMII_NFIELDS] = {
{ "temperature",		"outside",	"temperature",		1, 1 },
{ "temperature_min",		"outside",	"temperature_min",	0, 1 },
{ "temperature_max",		"outside",	"temperature_max",	0, 1 },
{ "temperature_inside",		"inside",	"temperature",		1, 1 },
{ "temperature_inside_min",	"inside",	"temperature_min",	0, 1 },
{ "temperature_inside_max",	"inside",	"temperature_max",	0, 1 },
{ "humidity",			"outside",	"humidity",		1, 1 },
{ "humidity_min",		"outside",	"humidity_min",		0, 1 },
{ "humidity_max",		"outside",	"humidity_max",		0, 1 },
{ "humidity_inside",		"inside",	"humidity",		1, 1 },
{ "humidity_inside_min",	"inside",	"humidity_min",		0, 1 },
{ "humidity_inside_max",	"inside",	"humidity_max",		0, 1 },
{ "barometer",			"inside",	"barometer",		1, 1 },
{ "barometer_min",		"inside",	"barometer_min",	0, 1 },
{ "barometer_max",		"inside",	"barometer_max",	0, 1 },
{ "rain",			"outside",	"rain",			1, 1 },
{ "raintotal",			"outside",	"raintotal",		1, 0 },
{ "windspeed",			"outside",	"wind",			1, 1 },
{ "winddir",			"outside",	"winddir",		1, 1 },
{ "windgust",			"outside",	"windgust",		1, 1 },
{ "windx",			"outside",	"windx",		1, 0 },
{ "windy",			"outside",	"windy",		1, 0 },
{ "duration",			"outside",	"duration",		1, 0 },
{ "samples",			"outside",	"samples",		1, 0 }
};

class fieldid {
public:
	int	sensorid;
	int	mfieldid;
	fieldid(int s, int m) : sensorid(s), mfieldid(m) { }
};

// retrieve a field id from the database, based on stationname, sensorname
// and mfieldname. Note that sensorname and mfieldname can be taken from
// the map tables defined above
static fieldid	getfieldid(MYSQL *mysql, const char *stationname,
		const char *sensorname, const char *mfieldname) {
	char	query[1024];
	snprintf(query, sizeof(query),
		"select b.id as sensorid, c.id as mfieldid "
		"from station a, sensor b, mfield c "
		"where a.name = '%s' "
		"  and a.id = b.stationid "
		"  and b.name = '%s' "
		"  and c.name = '%s'",
		stationname, sensorname, mfieldname);
	if (debug > 1)
		fprintf(stderr, "%s:%d: fieldid query for %s.%s.%s: %s\n",
			__FILE__, __LINE__,
			stationname, sensorname, mfieldname, query);
	mysql_query(mysql, query);
	MYSQL_RES	*res = mysql_store_result(mysql);
	if (1 != mysql_num_rows(res)) {
		fprintf(stderr, "%s:%d: no id found for field %s\n", __FILE__,
			__LINE__, mfieldname);
		exit(EXIT_FAILURE);
	}
	MYSQL_ROW	row = mysql_fetch_row(res);
	int	sensorid = atoi(row[0]);
	int	mfieldid = atoi(row[1]);
	if (debug)
		fprintf(stderr, "%s:%d: fieldid for %s is (%d, %d)\n", __FILE__,
			__LINE__, mfieldname, sensorid, mfieldid);
	mysql_free_result(res);
	return fieldid(sensorid, mfieldid);
}

// getallfieldids retrieves sensor and mfield data from the database and returns
// an array of fieldid's. These are pairs of sensor ids and mfield ids
static fieldid	*getallfieldids(MYSQL *mysql, const char *stationname,
				const fieldmap_t *fm, int nfields) {
	if (debug)
		fprintf(stderr, "%s:%d: looking for %d fields\n",
			__FILE__, __LINE__, nfields);
	// allocate a map for the new fields
	fieldid *result = (fieldid *)malloc(nfields * sizeof(fieldid));

	// insert field ids into the table
	for (int i = 0; i < nfields; i++) {
		// get the fieldid for this field
		result[i] = getfieldid(mysql, stationname,
			fm[i].sensorname, fm[i].mfieldname);
	}

	// return the table with all the field ids
	return result;
}

// insert a header row, there does not exist anything equivalent in the
// old architecture, so we can just do this
static void	insertheader(MYSQL *mysql, time_t t) {
	char	query[1024];
	// first check whether this particular key value already exists	
	snprintf(query, sizeof(query),
		"select count(*) from header where timekey = %ld", t);
	mysql_query(mysql, query);
	MYSQL_RES	*res = mysql_store_result(mysql);
	MYSQL_ROW	row = mysql_fetch_row(res);
	int	exists = atoi(row[0]);
	mysql_free_result(res);

	// if no row exists, add one
	if (exists) {
		if (debug)
			fprintf(stderr, "%s:%d: returning without header add\n",
				__FILE__, __LINE__);
		return;
	}
	// time timekey marks the end of the interval, so when we compute the
	// group, we should substract the interval and add the offset
	int	group300 = (t + offset) / 300;
	int	group1800 = (t + offset) / 1800;
	int	group7200 = (t + offset) / 7200;
	int	group86400 = (t + offset) / 86400;
	snprintf(query, sizeof(query),
		"insert into header(timekey, group300, group1800, group7200, "
		"group86400) values (%ld, %d, %d, %d, %d)",
		t, group300, group1800, group7200, group86400);
	if (debug)
		fprintf(stderr, "%s:%d: header insert: %s\n",
			__FILE__, __LINE__, query);
	if (mysql_query(mysql, query)) {
		if (!quiet)
			fprintf(stderr,
				"%s:%d: WARNING: header insert failed: %s\n",
				__FILE__, __LINE__, mysql_error(mysql));
	}
}

// insert a single row the sdata table.
static void	insertsrow(MYSQL *mysql, time_t t, const fieldid& sfi,
			const char *value) {
	char	query[1024];
	if (debug)
		fprintf(stderr, "%s:%d: value for (%d,%d) at %p\n",
			__FILE__, __LINE__, sfi.sensorid, sfi.mfieldid, value);
	// we don't insert anything if the original table contained a NULL
	if (value == NULL)
		return;

	// convert to double, and build a query that is acceptable to mysql
	double	v = atof(value);
	snprintf(query, sizeof(query),
		"insert into sdata(timekey, sensorid, fieldid, value) "
		"values (%ld, %d, %d, %.5f)",
		t, sfi.sensorid, sfi.mfieldid, v);
	if (debug)
		fprintf(stderr, "%s:%d: query is %s\n", __FILE__, __LINE__,
			query);

	// send query to database
	if (mysql_query(mysql, query)) {
		if (!quiet)
			fprintf(stderr, "%s:%d: WARNING: value not inserted: "
				"%d,%d -> %s (%s): %s\n", __FILE__, __LINE__,
				sfi.sensorid, sfi.mfieldid, value, query,
				mysql_error(mysql));
	}
}

// insert a single row into the avg table
static void	insertarow(MYSQL *mysql, time_t t, int intval,
			const fieldid& sfi, const char *value) {
	char	query[1024];
	if (value == NULL)
		return;
	double	v = atof(value);
	snprintf(query, sizeof(query), "insert into avg(timekey, intval, "
		"sensorid, fieldid, value) values (%ld, %d, %d, %d, %.5f)",
		t + offset, intval, sfi.sensorid, sfi.mfieldid, v);
	if (mysql_query(mysql, query)) {
		if (!quiet)
			fprintf(stderr, "%s:%d: WARNING: value not inserted: "
				"(%d,%d) -> %s (%s): %s\n", __FILE__, __LINE__,
				sfi.sensorid, sfi.mfieldid, value, query,
				mysql_error(mysql));
	}
}

// read a chunk of data from the stationdate table
static int	getschunk(MYSQL *mysql, const std::string& station,
			int t, const fieldmap_t *map,
			const fieldid *idlist, int nfields, int chunksize) {
	if (debug)
		fprintf(stderr, "%s:%d: getchunk %d/%d\n", __FILE__, __LINE__,
			t, chunksize);
	char	query[1024];
	int	endtime;

	// perform a stationdata query for this station and at most chunksize
	// records
	strcpy(query, "select timekey");
	for (int i = 0; i < nfields; i++) {
		// only take the fields about which we know that they are
		// in the stationdata table
		if (map[i].instationdata) {
			strcat(query, ", ");
			if (map[i].fieldname == "windx") {
				strcat(query, "windx * duration");
			} else if (map[i].fieldname == "windy") {
				strcat(query, "windy * duration");
			} else {
				strcat(query, map[i].fieldname);
			}
		}
	}
	int o = strlen(query);
	snprintf(query + o, sizeof(query) - o, " "
		"from stationdata "
		"where station = '%-8.8s' "
		"  and timekey > %d order by timekey limit %d",
		station.c_str(), t, chunksize);
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
		fprintf(stderr, "%s:%d: found %d rows in stationdata\n",
			__FILE__, __LINE__, nrows);
	stationdata += nrows;
	MYSQL_ROW	row;
	while (NULL != (row = mysql_fetch_row(res))) {
		endtime = t = atoi(row[0]);
		if (debug)
			fprintf(stderr, "%s:%d: stationdata timekey: %d\n",
				__FILE__, __LINE__, t);
		insertheader(mysql, t);
		for (int i = 0, j = 1; i < nfields; i++) {
			if (map[i].instationdata) {
				insertsrow(mysql, t, idlist[i],
					row[j++]);
			}
		}
		stationdata++;
		endtime = atoi(row[0]);
	}

	// free the result from this query
	mysql_free_result(res);

	// tell caller where we are
	return endtime;
}

// get a chunk of data from the averages table
static int	getachunk(MYSQL *mysql, const std::string& station,
			int t, const fieldmap_t *map,
			const fieldid *idlist, int nfields, int chunksize) {
	if (debug)
		fprintf(stderr, "%s:%d: getachunk %d/%d\n", __FILE__, __LINE__,
			t, chunksize);
	char	query[1024];
	int	endtime;

	// perform an averages query for this station and the interval starting
	// at timekey and ending at the timekey of the last record of the
	// previous query
	strcpy(query, "select timekey");
	for (int i = 0; i < nfields; i++) {
		// only take the fields about which we know that they are
		// in the stationdata table
		if (map[i].inaverages) {
			strcat(query, ", ");
			strcat(query, map[i].fieldname);
		}
	}
	int	o = strlen(query);
	snprintf(query + o, sizeof(query) - o, " "
		"from averages "
		"where station = '%-8.8s' "
		"  and timekey > %d order by timekey limit %d",
		station.c_str(), t, chunksize);
	if (debug)
		fprintf(stderr, "%s:%d: averages query: %s\n",
			__FILE__, __LINE__, query);
	if (mysql_query(mysql, query)) {
		fprintf(stderr, "%s:%d: cannot perform query %s: %s\n",
			__FILE__, __LINE__, query, mysql_error(mysql));
		exit(EXIT_FAILURE);
	}

	// retrieve result for averages
	MYSQL_RES	*res = mysql_store_result(mysql);
	int	nrows = mysql_num_rows(res);
	if (debug)
		fprintf(stderr, "%s:%d: found %d rows in averages\n",
			__FILE__, __LINE__, nrows);
	averages += nrows;
	MYSQL_ROW	row;
	while ((row = mysql_fetch_row(res))) {
		int	intval = atoi(row[2]);
		endtime = t = atoi(row[0]);
		for (int i = 0, j = 1; i < nfields; i++) {
			if (map[i].inaverages) {
				insertarow(mysql, t, intval, idlist[i],
					row[j++]);
			}
		}
		averages++;
	}

	// free the result from this query
	mysql_free_result(res);

	// return the new last records timekey
	return endtime;
}

time_t	firstkey(MYSQL *mysql, const std::string& station) {
	time_t	result = 0;

	// build a query
	char	query[1023];
	snprintf(query, sizeof(query),
		"select min(timekey) "
		"from stationdata "
		"where station = '%-8.8s'", station.c_str());
	if (debug)
		fprintf(stderr, "%s:%d: looking for first key: %s",
			__FILE__, __LINE__, query);
	if (mysql_query(mysql, query)) {
		fprintf(stderr, "%s:%d: cannot perform query %s: %s\n",
			__FILE__, __LINE__, query, mysql_error(mysql));
		exit(EXIT_FAILURE);
	}

	// retrieve the result
	MYSQL_RES	*res = mysql_store_result(mysql);
	if (mysql_num_rows(res) == 0) {
		goto haveit;
	}
	{
		MYSQL_ROW	row = mysql_fetch_row(res);
		if (row[0] == NULL) return 0;
		result = atoi(row[0]);
	}
haveit:
	// free the result from this query
	mysql_free_result(res);
	if (debug)
		fprintf(stderr, "%s:%d: first key: %ld", __FILE__, __LINE__,
			result);
	return result;
}

// main program
int	main(int argc, char *argv[]) {
	std::string	hostname("localhost");
	std::string	user("meteo");
	std::string	password("public");
	std::string	database("meteo");
	std::string	station;
	std::string	basename("./");
	std::string	logurl("file:///-");
	std::string	stationtype;
	int	t = 0;
	int	chunksize = 1440; 	// one day worth of data
	fieldmap_t	*map = NULL;
	int	mapsize = 0;
	MYSQL	mysql;
	int	c;

	// parse the command line
	while (EOF != (c = getopt(argc, argv, "db:h:l:o:u:p:s:t:c:f:q")))
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
		case 'o':
			offset = atoi(optarg);
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
			station = std::string(optarg);
			break;
		case 't':
			stationtype = std::string(optarg);
			break;
		case 'c':
			chunksize = atoi(optarg);
			break;
		case 'f':
			t = atoi(optarg);
			break;
		case 'q':
			quiet = true;
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

	// decide which map table to use
	if (stationtype == "VantagePro") {
		map = vantagemap;
		mapsize = VANTAGE_NFIELDS;
	}
	if (stationtype == "WMII") {
		map = wmiimap;
		mapsize = WMII_NFIELDS;
	}
	if (NULL == map) {
		fprintf(stderr, "station type not set\n");
		exit(EXIT_FAILURE);
	}

	// read field ids from the database, this also depends on the
	// station name, as the fieldid contains the sensorid
	fieldid	*idlist = getallfieldids(&mysql, station.c_str(), map, mapsize);
	if (debug)
		fprintf(stderr, "%s:%d: station %s\n", __FILE__,
			__LINE__, station.c_str());

	// work on stationdata
	if (t == 0)
		t = firstkey(&mysql, station);
	if (debug)
		fprintf(stderr, "%s:%d: trying stationdata\n",
			__FILE__, __LINE__);
	do {
		int	oldt = t;
		t = getschunk(&mysql, station.c_str(), t, map,
			idlist, mapsize, chunksize);
		if (t == oldt)
			break;
	} while (t > 0);

	// work on averages
	t = firstkey(&mysql, station);
	if (debug)
		fprintf(stderr, "%s:%d: trying averages\n",
			__FILE__, __LINE__);
	do {
		int	oldt = t;
		t = getachunk(&mysql, station.c_str(), t, map,
			idlist, mapsize, chunksize);
		if (t == oldt)
			break;
	} while (t > 0);

	// disconnect from database
	if (debug)
		fprintf(stderr, "%s:%d: disconnecting from database\n",
			__FILE__, __LINE__);
	mysql_close(&mysql);

	// that's it
	exit(EXIT_SUCCESS);
}
