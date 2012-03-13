/*
 * QueryProcessor.cc -- perform database queries against 
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: QueryProcessor.cc,v 1.17 2004/02/27 16:03:50 afm Exp $
 */
#include <QueryProcessor.h>
#include <Configuration.h>
#include <mysql.h>
#include <mdebug.h>
#include <MeteoException.h>
#include <StationInfo.h>
#include <ValueFactory.h>
#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#endif

namespace meteo {

//////////////////////////////////////////////////////////////////////
// QueryProcessor_internal class
class QueryProcessor_internal {
	static MYSQL	*romysql;
	static MYSQL	*rwmysql;
	bool	readwrite;
	MYSQL	*connect(void);
public:
	QueryProcessor_internal(bool rw);
	~QueryProcessor_internal(void);
	void	perform(const Query& query, QueryResult& result);
	int	perform(const std::string& query);
	Datarecord	nearRecord(const time_t, bool backwards,
		int window = 3600);
	BasicQueryResult	operator()(const std::string& query);
	void	close(void);
	friend class QueryProcessor;
};

MYSQL	*QueryProcessor_internal::romysql = NULL;
MYSQL	*QueryProcessor_internal::rwmysql = NULL;

// the constructor does not connect the query processor to the database,
// instead it waits until a real database call comes along, the connection
// is then established by the connect method
MYSQL	*QueryProcessor_internal::connect(void) {
	MYSQL	*lmysql = (readwrite) ? rwmysql : romysql;
	if (NULL != lmysql) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "%p already connected", this);
		return lmysql;
	}

	// create a new mysql structure
	lmysql = mysql_init(NULL);

	// connection needed
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "build connection %p", this);
	Configuration	conf;

	// initialize database structure
	mysql_init(lmysql);

	// get database connection parameters from the configuration
	std::string	host
		= conf.getString("/meteo/database/hostname", "meteo");
	std::string	dbname
		= conf.getString("/meteo/database/dbname", "meteo");
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "%p, read/write flag is %s", this,
		(readwrite) ? "true" : "false");
	std::string	user
		= conf.getString((readwrite) ? "/meteo/database/writer"
					     : "/meteo/database/user",
			"meteo");
	std::string	passwd
		= conf.getString((readwrite) ? "/meteo/database/writerpassword"
					     : "/meteo/database/password",
			"public");

	// connect to the database
	if (NULL == mysql_real_connect(lmysql, host.c_str(),
		user.c_str(), passwd.c_str(), dbname.c_str(),
		0, NULL, 0)) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "%p, cannot connect to "
			"database: %s", this, mysql_error(lmysql));
		std::string	err(mysql_error(lmysql));
		mysql_close(lmysql);	// prevent memory leak
		throw MeteoException("cannot connect to database",
			mysql_error(lmysql));
	}

	// remember the correct mysql database handle
	if (readwrite) {
		rwmysql = lmysql;
	} else {
		romysql = lmysql;
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "%p connected to database", this);
	return lmysql;
}


// constructor for QueryProcessor_internal: builds the database connection
// from the information usually found in the xml configuration file
QueryProcessor_internal::QueryProcessor_internal(bool rw) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "MM %p QueryProcessor_internal create",
		this);
	// make sure we don't believe the mysql connection is ready
	readwrite = rw;
}

QueryProcessor_internal::~QueryProcessor_internal(void) {
}

// perform a Query on the database and resturn the QueryResult
void	QueryProcessor_internal::perform(const Query& query,
	QueryResult& result) {
	// make sure we have a connection
	MYSQL	*mysql = connect();

	// let the Query class create the complete query
	std::string	qs = query.getQuery();

	// this query has retrieves timekey, sensorid, fieldid and value
	// for all the selected sensorid/mfieldid combinations (as specified
	// by the fully qualified name in the XML config file);

	// the QueryResult is essentially a mapt that maps names to Tdata
	// objects. They key to this map are the symbolic names, not the
	// fully qualified names. To construct this map, we go through
	// the select member of the Query object, becaus it is a map from
	// symbolic names to fully qualified names
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "building result dmap_t");
	smap_t::const_iterator	qi;
	for (qi = query.select.begin(); qi != query.select.end(); qi++) {
		// log which field we are processing, qi->second is the
		// fully qualified field name we are supposed to be working on
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"%p, storing data for field %s->%s", this,
			qi->first.c_str(), qi->second.c_str());

		// add a new Tdata element to the result (a QueryResult object)
		Tdata	v(query.getInterval(),
			query.getStart(), query.getEnd());
		result.data.insert(dmap_t::value_type(qi->first, v));
		// so far, the new Tdata stuff is completely empty
	}

	// perform the query against the database
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "%p query: %s", this, qs.c_str());
	if (mysql_query(mysql, qs.c_str())) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0,
			"%p, cannot retrieve data for %s: %s", this, qs.c_str(),
			mysql_error(mysql));
		throw MeteoException("cannot perform query", qs);
	}

	// retrieve data from the database, just store everything in the
	// mres variable
	MYSQL_RES	*mres;
	mres = mysql_store_result(mysql);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "%p, %d rows found", this,
		mysql_num_rows(mres));

	// now go through the result set and store the data in these
	// arrays, access to the arrays is now very fast (no maps)
	MYSQL_ROW	row;
	while (NULL != (row = mysql_fetch_row(mres))) {
		// convert fields from text (as returned by mysql) to native
		// types, the timekey and value are simple
		int	timekey = atoi(row[0]);
		double	value = atof(row[3]);

		// to access the other fields, we need to convert the pair
		// (sensorid,mfieldid) to a fieldid (since this is the key
		// to the idmap member of the Query object)
		fieldid	id = FQField().getFieldid(atoi(row[1]), atoi(row[2]));
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "%d,%.2f id = %d,%d,%d",
			timekey, value, id.stationid, id.sensorid, id.mfieldid);

		// find the data name in the idmap table, the image under
		// the idmap is the symbolic name for the data, i.e. we can
		// use it to retrieve the right Tdata
		imap_t::const_iterator	i = query.idmap.find(id);
		if (i == query.idmap.end()) {
			mdebug(LOG_ERR, MDEBUG_LOG, 0, "id (%d,%d,%d) not "
				"found, Query class corrupt", id.stationid,
				id.sensorid, id.mfieldid);
			throw MeteoException("Query class corrupt", "");
		}
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "symbolic name: %s",
			i->second.c_str());
		Tdata&	vc = result.data[i->second];

		// add the value retrieved to the Tdata
		vc[timekey] = value;
	}
	mysql_free_result(mres); mres = NULL;
}

int	QueryProcessor_internal::perform(const std::string& query) {
	// make sure we have a connection
	MYSQL	*mysql = connect();
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "%p query: %s", this, query.c_str());

	if (mysql_query(mysql, query.c_str())) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "%p, query %s failed: %s",
			this, query.c_str(),
			mysql_error(mysql));
		throw MeteoException(mysql_error(mysql), query.c_str());
	}

	// handle the result, if there is any
	MYSQL_RES	*mres = mysql_store_result(mysql);
	int	rows = mysql_affected_rows(mysql);
	mysql_free_result(mres);

	// return number of affected rows
	return rows;
}


BasicQueryResult QueryProcessor_internal::operator()(const std::string& query) {
	// make sure we have a connection
	MYSQL	*mysql = connect();

	// log the query
	const char	*q = query.c_str();
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "%p query: %s", this, q);

	// send the query to the database
	if (mysql_query(mysql, q)) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0,
			"cannot retrieve data for %s: %s", q,
			mysql_error(mysql));
		throw MeteoException("cannot perform query", query);
	}

	// retrieve data from the database
	MYSQL_RES	*mres;
	mres = mysql_store_result(mysql);

	// for UPDATE/INSERT it is ok to return NULL, but we should only
	// work on mres if it is not NULL. But since we have to return a
	// BasicQueryResult, we create one here, even if we don't fill in
	// anything
	BasicQueryResult	result;
	if (NULL != mres) {
		// retrieve the result set (since NULL != mres we know there
		// is some result set)
		MYSQL_ROW	row;
		int		nfields = mysql_num_fields(mres);
		while (NULL != (row = mysql_fetch_row(mres))) {
			Row	r;
			for (int i = 0; i < nfields; i++) {
				if (row[i] != NULL)
					r.push_back(std::string(row[i]));
				else
					r.push_back(std::string("NULL"));
			}
			result.push_back(r);
		}
		int	nrows = mysql_num_rows(mres);
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "%p, %d rows found",
			this, nrows);
		// free 
		mysql_free_result(mres);
	}

	// return the result
	return result;
}
//////////////////////////////////////////////////////////////////////
// QueryProcessor methods
QueryProcessor::QueryProcessor(bool readwrite, bool ig) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "MM %p QueryProcessor create", this);
	internal = new QueryProcessor_internal(readwrite); // delete ~
	ignoreerrors = ig;
}

QueryProcessor::~QueryProcessor(void) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "MM %p QueryProcessor destroy", this);
	delete internal;
}

void	QueryProcessor::perform(const Query& q, QueryResult& r) {
	try {
		internal->perform(q, r);
	} catch (MeteoException& me) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "perform failed: %s, %s",
			me.getReason().c_str(), me.getAddinfo().c_str());
		if (!ignoreerrors)
			throw;
	}
}

// retrieve the nearset record, searching forward or backward, that has
// data for the corresponding station
Datarecord	QueryProcessor::nearRecord(const time_t timekey,
			const std::string& stationname, bool backwards,
			int window) {
	char	query[2048];
	// the result is returned as a Datarecord, so we prepare one
	Datarecord	result;

	// get the station info for this station
	StationInfo	si(stationname);

	// find the set of sensor ids for this station
	intlist	sensorids = si.getSensorIds();
	std::string	sensorclause("(");
	intlist::iterator	i;
	for (i = sensorids.begin(); i != sensorids.end(); i++) {
		if (i != sensorids.begin())
			sensorclause += ", ";
		char	idchr[10];
		snprintf(idchr, sizeof(idchr), "%d", *i);
		sensorclause += idchr;
	}
	sensorclause += ")";

	// build a query to retrieve the last time stamp
	time_t	limit = timekey + ((backwards) ? -1 : 1) * window;
	snprintf(query, sizeof(query),
		"select %s(timekey) "
		"from sdata "
		"where timekey between %ld and %ld "
		"  and sensorid in %s ",
		(backwards) ? "max" : "min",
		(backwards) ? limit : timekey,
		(backwards) ? timekey : limit,
		sensorclause.c_str()
	);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "nearRecord query: %s", query);
	BasicQueryResult	bqr = this->operator()(query);

	// we have a problem if there is no last timekey
	if (bqr.size() == 0) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "no timekey found");
		if (!ignoreerrors) {
			throw MeteoException("no timekey found", "");
		}
	}

	// extract the timekey from the result
	result.setTimekey(atoi((*bqr.begin())[0].c_str()));

	// retrieve all the data fields together with the class information
	snprintf(query, sizeof(query),
		"select a.value, c.name, c.id, c.class, c.unit, b.id "
		"from sdata a, sensor b, mfield c "
		"where a.timekey = %ld "
		"  and a.sensorid = b.id "	// a.id is stationid
		"  and a.fieldid = c.id "	// a.fieldid is mdfield id
		"  and b.id in %s",		// b.id is the sensorid
		result.getTimekey(), sensorclause.c_str());
	bqr = this->operator()(query);

	// create Value objects from the data and add it to the Datarecord,
	// note that Wind and Rain have to be treated specially
	BasicQueryResult::iterator	j;
	bool		haswind = false;
	double		windspeed, winddir = 0.;
	std::string	windunit, windname;
	for (j = bqr.begin(); j != bqr.end(); j++) {
		// the fieldname value retrieved here is the mfield name,
		// later on we will need the fully qualified field name
		std::string	fieldname = (*j)[1];
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "adding field %s",
			fieldname.c_str());

		// decode data
		double	value = atof((*j)[0].c_str());		// a.value
		int	mfieldid = atoi((*j)[2].c_str());	// c.id
		std::string	classname = (*j)[3];		// c.class
		std::string	unit = (*j)[4];			// c.unit
		int	sensorid = atoi((*j)[5].c_str());	// b.id

		// using sensorid and mfieldid, retrieve the complete 
		// fqfieldname
		fieldid	fid = FQField().getFieldid(sensorid, mfieldid);
		fqfieldname	fqfn = FQField().getFqfieldname(fid);
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "fq name: %s", 
			fqfn.getString().c_str());

		// skip fields with NULL class, they are not convertible to
		// a datarecord
		if (classname == "NULL")
			continue;

		// construct a value, and add it to the Datarecord, remember
		// wind data for later
		if (fieldname == "wind") {
			haswind = true;
			windspeed = value;
			windunit = unit;
			windname = fqfn.getString();
			break;
		}
		if (fieldname == "winddir") {
			winddir = value;
			break;
		}

		// skip all other fields that begin with "wind"
		Value	v = ValueFactory::getValue(classname, value, unit);
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "adding value %s: %f %s",
			fieldname.c_str(), v.getValue(), v.getUnit().c_str());
		result.addField(fqfn.getString(), v);
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "done with %s", 
			fqfn.getString().c_str());
	}
	if (haswind) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "creating wind %s",
			windname.c_str());
		Value	v = ValueFactory::getValue(Vector(windspeed,
				(int)winddir, true), windunit);
		result.addField(windname, v);
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "wind added");
	}

	// return the datarecord
	return result;
}

BasicQueryResult	QueryProcessor::operator()(const std::string& query) {
	return internal->operator()(query);
}

int	QueryProcessor::perform(const std::string& query) {
	try {
		return internal->perform(query);
	} catch (MeteoException& me) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "perform '%s' failed: %s, %s",
			query.c_str(), me.getReason().c_str(),
			me.getAddinfo().c_str());
		if (!ignoreerrors)
			throw;
	}
	return 0;
}
int	QueryProcessor::perform(const char *query) {
	return perform(std::string(query));
}

} /* namespace meteo */
