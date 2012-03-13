/*
 * Query.cc -- perform database queries against 
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#include <Query.h>
#include <mysql.h>
#include <mdebug.h>
#include <MeteoException.h>
#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#endif

namespace meteo {

//////////////////////////////////////////////////////////////////////
// Query class
std::string	Query::getSelectClause(void) const {
	std::string	result;
	smap_t::const_iterator	i;
	for (i = select.begin(); i != select.end(); i++) {
		if (i != select.begin())
			result += ", ";
		result += i->second;
	}
	return result;
}

std::string	Query::getWhereClause(void) const {
	char	b[1024];
	if (interval != 60) {
		snprintf(b, sizeof(b),
			" from averages "
			"where timekey >= %ld "
			"  and timekey <= %ld "
			"  and intval = %d ", start, end, interval);
	} else {
		snprintf(b, sizeof(b),
			" from stationdata "
			"where timekey >= %ld "
			"  and timekey <= %ld ",
			start, end);
	}
	return b;
}

std::string	Query::getQuery(void) const {
	std::string	result = "select timekey, "
		+ getSelectClause() + getWhereClause();
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "query is %s", result.c_str());
	return result;
}

//////////////////////////////////////////////////////////////////////
// QueryProcessor_internal class
class QueryProcessor_internal {
	MYSQL			mysql;
	MYSQL_RES		*mres;
	std::string		stationname;
	const Configuration&	config;
public:
	QueryProcessor_internal(const Configuration& conf,
		const std::string& name);
	~QueryProcessor_internal(void);
	void	perform(const Query& query, QueryResult& result);
	Datarecord	nearRecord(const time_t, bool backwards,
		int window = 3600);
};

QueryProcessor_internal::QueryProcessor_internal(const Configuration& conf,
	const std::string& name) : config(conf), stationname(name) {
	// initialize database structure
	mysql_init(&mysql);
	mres = NULL;

	// get database connection parameters from the configuration
	std::string	host
		= conf.getString("/meteo/database/hostname", "meteo");
	std::string	dbname
		= conf.getString("/meteo/database/dbname", "meteo");
	std::string	user
		= conf.getString("/meteo/database/user", "meteo");
	std::string	passwd
		= conf.getString("/meteo/database/password", "public");

	// connect to the database
	if (NULL == mysql_real_connect(&mysql, host.c_str(),
		user.c_str(), passwd.c_str(), dbname.c_str(),
		0, NULL, 0)) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "cannot connect to "
			"database: %s", mysql_error(&mysql));
		throw MeteoException("cannot connect to database",
			mysql_error(&mysql));
	}
}

QueryProcessor_internal::~QueryProcessor_internal(void) {
	if (NULL != mres)
		mysql_free_result(mres);
	mysql_close(&mysql);
}

void	QueryProcessor_internal::perform(const Query& query,
	QueryResult& result) {
	int	i, j, k;
	int	nfields = query.getSelectCount();
	std::string	qs = query.getQuery() + "and station = '"
				+ stationname.substr(0, 8) + "'";

	// perform the query against the database
	if (mysql_query(&mysql, qs.c_str())) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0,
			"cannot retrieve data for %s: %s", qs.c_str(),
			mysql_error(&mysql));
		throw MeteoException("cannot perform query", qs);
	}

	// retrieve data from the database
	mres = mysql_store_result(&mysql);
	int		nrows = mysql_num_rows(mres);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "%d rows found", nrows);

	// we first store the result of the query in a large
	// temporary array because we would otherwise have to
	// traverse the map column_name -> Tdata many many times,
	// hurting performance significantly
	time_t	*keys = (time_t *)alloca(nrows*sizeof(time_t));
	bool	*isnull = (bool *)alloca(nrows*nfields*sizeof(bool));
	double	*rdata = (double *)alloca(nrows*nfields*sizeof(double));
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "space allocated for keys %p, "
		"isnull %p, rdata %p", (void *)keys, (void *)isnull,
		(void *)rdata);

	// now go through the result set and store the data in these
	// arrays, access to the arrays is now very fast (no maps)
	int	nonnull = 0;
	for (i = 0; i < nrows; i++) {
		MYSQL_ROW	row = mysql_fetch_row(mres);
		// retrieve and normalize the key
		keys[i] = atoi(row[0]);
		keys[i] -= keys[i] % query.getInterval();
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "working on row %d/%d",
			i, keys[i]);
		for (j = 0; j < nfields; j++) {
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
				"working on field %d", j);
			k = i * nfields + j;
			isnull[k] = (NULL == row[j + 1]);
			if (!isnull[k]) {
				rdata[k] = atof(row[j + 1]);
				mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
					"data[%d, %d ~ %d] = %f",
					i, j, k, rdata[k]);
				nonnull++;
			} else {
				mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
					"data[%d, %d ~ %d] is NULL",
					i, j, k);
			}
		}
	}
	mysql_free_result(mres); mres = NULL;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "retrieved %d non-null items",
		nonnull);

	// no go through the arrays in the other direction an build
	// the result maps
	smap_t::const_iterator	qi;
	for (qi = query.select.begin(), j = 0;
		qi != query.select.end(); qi++, j++) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"storing data for field %s->%s",
			qi->first.c_str(), qi->second.c_str());

		// add a new Tdata element
		Tdata	v(query.getInterval(),
			query.getStart(), query.getEnd());
		result.data[qi->first] = v;

		// get a reference to the Tdata
		Tdata&	vc = result.data[qi->first];

		// fill in the data
		for (i = 0; i < nrows; i++) {
			k = i * nfields + j;
			if (!isnull[k])
				vc[keys[i]] = rdata[k];
		}
	}	
}

Datarecord	QueryProcessor_internal::nearRecord(const time_t timekey,
	bool backwards, int window) {
	// build a query that finds the timestamp of the closes record
	char	query[2048];
	time_t	limit = timekey + ((backwards) ? -1 : 1) * window;
	snprintf(query, sizeof(query),
		"select %s(timekey) "
		"from stationdata "
		"where timekey between %ld and %ld "
		"  and station = '%s' ",
		(backwards) ? "max" : "min",
		(backwards) ? limit : timekey,
		(backwards) ? timekey : limit,
		stationname.substr(0, 8).c_str()
	);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "nearRecord query: %s", query);

	// search for the timestamp
	if (mysql_query(&mysql, query)) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "query '%s' fails: %s",
			query, mysql_error(&mysql));
		throw MeteoException(query, mysql_error(&mysql));
	}
	mres = mysql_store_result(&mysql);
	if (NULL == mres) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0,
			"cannot store result to query '%s'", query);
		throw MeteoException("cannot store result",
			mysql_error(&mysql));
	}
	if (1 != mysql_num_rows(mres)) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "no data for timekey query"
			" '%s'", query);
		throw MeteoException("no data for timekey query",
			mysql_error(&mysql));
	}
	MYSQL_ROW	row = mysql_fetch_row(mres);
	if (row[0] == NULL) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "NULL max/min value returned");
		throw MeteoException("NULL max/min value", "");
	}
	time_t	realkey = atoi(row[0]);
	mysql_free_result(mres); mres = NULL;

	// build a query for a precise 
	snprintf(query, sizeof(query),
		"select	temperature, temperature_inside, "
		"	humidity, humidity_inside, "
		"	barometer, rain, raintotal, "
		"	windspeed, winddir, windgust, duration, "
		"	solar, uv, samples "
		"from stationdata where station = '%s' and timekey = %ld",
		stationname.substr(0, 8).c_str(), realkey);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "query for record: %s", query);

	if (mysql_query(&mysql, query)) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "record query failed: %s",
			mysql_error(&mysql));
		throw MeteoException("record query failed",
			mysql_error(&mysql));
	}

	mres = mysql_store_result(&mysql);
	row = mysql_fetch_row(mres);

	// prepare a result Datarecord
	Datarecord	rr;
	rr.setStationname(stationname);
	rr.setTimekey(realkey);

	// retrieve database units from the configuration
	rr.setTemperatureUnit(config.getTemperatureUnit());
	rr.setHumidityUnit(config.getHumidityUnit());
	rr.setPressureUnit(config.getPressureUnit());
	rr.setRainUnit(config.getWindUnit());
	rr.setWindUnit(config.getWindUnit());
	rr.setSolarUnit(config.getSolarUnit());
	rr.setUVUnit(config.getUVUnit());

	// start filling in the record
	if (row[0]) {	// temperature outside
		rr.temperatureoutside.setValue(atof(row[0]));
	}
	if (row[1]) {	// temperature inside
		rr.temperatureinside.setValue(atof(row[1]));
	}
	if (row[2]) {	// humdity outside
		rr.humidityoutside.setValue(atof(row[2]));
	}
	if (row[3]) {	// humdity inside
		rr.humidityinside.setValue(atof(row[3]));
	}
	if (row[4]) {	// barometer
		rr.barometer.setValue(atof(row[4]));
	}
	if (row[5]) {	// rain
		rr.rain.setValue(atof(row[5]));
		rr.rain.setTotal(atof(row[6]));
	}
	if (row[7]) {	// wind
		unsigned short	azi = (unsigned short)atof(row[8]);
		rr.wind.setValue(Vector(atof(row[7]), azi));
		rr.wind.setMax(Vector(atof(row[9]), azi));
	}
	if (row[11]) {	// solar
		rr.solar.setValue(atof(row[11]));
	}
	if (row[12]) {	// uv
		rr.uv.setValue(atof(row[12]));
	}

	// return the record we have constructed
	return rr;
}

QueryProcessor::QueryProcessor(const Configuration& conf,
	const std::string& name) : stationname(name), configuration(conf) {
	internal = new QueryProcessor_internal(configuration, name);
}

void	QueryProcessor::perform(const Query& q, QueryResult& r) {
	internal->perform(q, r);
}

Datarecord	QueryProcessor::lastRecord(const time_t notafter, int window) {
	return internal->nearRecord(notafter, true, window);
}

Datarecord	QueryProcessor::firstRecord(const time_t notbefore, int window) {
	return internal->nearRecord(notbefore, false, window);
}

//////////////////////////////////////////////////////////////////////
// QueryResult class
//////////////////////////////////////////////////////////////////////

bool	QueryResult::existsItem(const std::string& dataname, time_t timekey) const {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "existence query for data %s at %d",
		dataname.c_str(), timekey);
	dmap_t::const_iterator	i = data.find(dataname);
	if (i == data.end())
		return false;
	const tdata_t&	td = i->second.getData();
	return (td.find(timekey) != td.end());
}

double	QueryResult::fetchItem(const std::string& dataname, time_t timekey) const {
	dmap_t::const_iterator	i = data.find(dataname);
	if (i == data.end()) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0,
			"data name %s does not exist", dataname.c_str());
		throw MeteoException("data item does not exist", "");
	}
	const tdata_t&	td = i->second.getData();
	tdata_t::const_iterator	j = td.find(timekey);
	if (j == td.end()) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0,
			"data item %s at %d does not exist", dataname.c_str(),
				timekey);
		throw MeteoException("data item does not exist", "");
	}
	return j->second;
}

} /* namespace meteo */
