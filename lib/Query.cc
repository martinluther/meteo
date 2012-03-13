/*
 * Query.cc -- perform database queries against 
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#include <Query.h>
#include <mysql.h>
#include <mdebug.h>
#include <MeteoException.h>

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
	MYSQL	mysql;
public:
	QueryProcessor_internal(const Configuration& conf) {
		// initialize database structure
		mysql_init(&mysql);

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
	QueryProcessor_internal(void) {
		mysql_close(&mysql);
	}
	void	perform(const std::string& stationname, const Query& query,
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
		MYSQL_RES	*res = mysql_store_result(&mysql);
		int		nrows = mysql_num_rows(res);
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
			MYSQL_ROW	row = mysql_fetch_row(res);
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
		mysql_free_result(res);
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
};

QueryProcessor::QueryProcessor(const Configuration& conf,
	const std::string& name) : stationname(name), configuration(conf) {
	internal = new QueryProcessor_internal(configuration);
}

void	QueryProcessor::perform(const Query& q, QueryResult& r) {
	internal->perform(stationname, q, r);
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
