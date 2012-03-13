/*
 * FQField_internals.cc -- implement field query oracle
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung 
 *
 * $Id: FQField_internals.cc,v 1.4 2004/02/27 16:03:50 afm Exp $
 */
#include <FQField_internals.h>
#include <MeteoException.h>
#include <map>
#include <SensorStationInfo.h>

namespace meteo {


FQField_internals::FQField_internals(void) : qp(false) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "FQField_internals");
}
FQField_internals::~FQField_internals(void) {
}

// assignment are explicitely forbidden
FQField_internals&	FQField_internals::operator=(const FQField_internals& other) {
	throw MeteoException("cannot assign FQField_internals", "");
	return *this;
}

// lookup(fieldid) -- retrieve the mfield specific information, i.e. an
//                    mfield_t and add it to the mfieldmap_t
fieldid	FQField_internals::lookup(const fieldid id) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "lookup(%d,%d,%d)", id.stationid,
		id.sensorid, id.mfieldid);

	// check whether the mfieldid already appears in the mfieldmap
	if (field2fqmap.find(id) != field2fqmap.end()) {
		return id;
	}

	// for an incomplete id, try to complete from the stationmap
	fieldid	id2 = id;
	if (stationmap.find(id) != stationmap.end()) {
		id2.stationid = stationmap.find(id2)->second;
		return id2;
	}

	// perform a query to retrieve name, unit, class
	char	query[1024];
	snprintf(query, sizeof(query),
		"select a.name, b.name, c.name, a.id "
		"from station a, sensor b, mfield c "
		"where a.id = b.stationid "
		"  and b.id = %d "
		"  and c.id = %d", id.sensorid, id.mfieldid);
	BasicQueryResult	bqr = qp(query);
	if (bqr.size() == 0) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0,
			"nothing returned for fieldid %d,%d,%d", id.stationid,
			id.sensorid, id.mfieldid);
		throw MeteoException("nothing returned for fieldid", "");
	}

	// build the fqfieldname
	fqfieldname	fq((*bqr.begin())[0], (*bqr.begin())[1],
				(*bqr.begin())[2]);
	fieldid	newid(atoi((*bqr.begin())[3].c_str()), id.sensorid,
			id.mfieldid);

	// store the mf structure in the cache
	field2fqmap.insert(field2fqfield_t::value_type(newid, fq));
	string2fieldmap.insert(string2field_t::value_type(fq.getString(),
		newid));

	// report about your work
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "lookup(%d,%d,%d) complete",
		id.stationid, id.sensorid, id.mfieldid);
	return newid;
}

// lookup based on fully qualified fieldname
fieldid	FQField_internals::lookup(const std::string& fieldname) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "lookup for %s", fieldname.c_str());

	// check whether the fqfn appears in the fieldid map, in which
	// case we just return
	if (string2fieldmap.find(fieldname) != string2fieldmap.end()) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "cache hit for %s",
			fieldname.c_str());
		return string2fieldmap[fieldname];
	}

	// the fieldname was not found in the cache, so we have to retrieve
	// the ids from the database
	fqfieldname	fq = split(fieldname);

	// perform a query to retrieve id, unit, class
	char	query[1024];
	snprintf(query, sizeof(query),
		"select a.id, b.id, c.id "
		"from station a, sensor b, mfield c "
		"where a.name = '%s' "
		"  and a.id = b.stationid "
		"  and b.name = '%s' "
		"  and c.name = '%s'",
		fq.stationname.c_str(), fq.sensorname.c_str(),
		fq.fieldname.c_str());

	BasicQueryResult	bqr = qp(query);
	if (bqr.size() == 0) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "nothing known about %s",
			fieldname.c_str());
		throw MeteoException("nothing known about field", fieldname);
	}
	fieldid	id(atoi((*bqr.begin())[0].c_str()),
			atoi((*bqr.begin())[1].c_str()),
			atoi((*bqr.begin())[2].c_str()));
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "found id %d,%d,%d for %s",
		id.stationid, id.sensorid, id.mfieldid, fieldname.c_str());

	// store the things in the cache
	field2fqmap.insert(field2fqfield_t::value_type(id, fq));
	string2fieldmap.insert(string2field_t::value_type(fq.getString(), id));

	// also store the stationid in the stationmap
	fieldid	id2 = id;
	id2.stationid = -1;
	stationmap.insert(field2station_t::value_type(id2, id.stationid));

	// return the id found
	return id;
}

// turn the pair sensorid/mfieldid into a fieldid
fieldid	FQField_internals::getFieldid(int sensorid, int mfieldid) {
	return lookup(fieldid(-1, sensorid, mfieldid));
}

// implementation of the station split function for fieldnames
fqfieldname	FQField_internals::split(const std::string& fqfn) const {
	fqfieldname	result;
	// check for qualified/unqualified field name
	size_t	i = fqfn.find('.');
	if (i == std::string::npos) {
		result.fieldname = fqfn;
		return result;
	}

	// continue splitting the fully qualified name into fields
	result.stationname = fqfn.substr(0, i);
	size_t	j = fqfn.find('.', i + 1);
	if (j == std::string::npos) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "need two `.' in fq names");
		throw MeteoException("fully qualified names need two `.'", "");
	}
	result.sensorname = fqfn.substr(i + 1, j - i - 1);
	result.fieldname = fqfn.substr(j + 1);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "%s split into %s|%s|%s",
		fqfn.c_str(),
		result.stationname.c_str(),
		result.sensorname.c_str(),
		result.fieldname.c_str());
	return result;
}

// generic Fieldid lookup function
fieldid	FQField_internals::getFieldid(const std::string& fieldname) {
	return lookup(fieldname);
}

fqfieldname	FQField_internals::getFqfieldname(const fieldid& fid) {
	lookup(fid);
	return field2fqmap.find(fid)->second;
}

} /* namespace meteo */
