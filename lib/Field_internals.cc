/*
 * Field_internals.cc -- implement field query oracle
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung 
 *
 * $Id: Field_internals.cc,v 1.4 2009/01/10 19:00:24 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <Field_internals.h>
#include <QueryProcessor.h>
#include <MeteoException.h>
#include <map>
#include <SensorStationInfo.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif /* HAVE_STDLIB_H */
#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif /* HAVE_STDIO_H */

namespace meteo {

// constructors and destructors

Field_internals::Field_internals(void) : qp(false) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "MM Field_internals %p create", this);
}
Field_internals::~Field_internals(void) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "MM Field_internals %p destroy", this);
}

// assignment are explicitely forbidden
Field_internals&	Field_internals::operator=(const Field_internals& other) {
	throw MeteoException("cannot assign Field_internals", "");
	return *this;
}

// lookup(mfieldid) -- retrieve the mfield specific information, i.e. an
//                     mfield_t and add it to the mfieldmap_t
int	Field_internals::lookup(const int mfieldid) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "lookup(%d)", mfieldid);

	// check whether the mfieldid already appears in the mfieldmap
	if (mfieldmap.find(mfieldid) != mfieldmap.end()) {
		return mfieldid;
	}

	// perform a query to retrieve name, unit, class
	char	query[1024];
	snprintf(query, sizeof(query),
		"select name, unit, class from mfield where id = %d",
		mfieldid);
	BasicQueryResult	bqr = qp(query);
	if (bqr.size() == 0) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0,
			"nothing returned for mfieldid %d", mfieldid);
		throw MeteoException("nothing returned for mfieldid", "");
	}

	// build the mfield_t
	mfield_t	mf;
	mf.name = (*bqr.begin())[0];
	mf.id = mfieldid;
	mf.unit = (*bqr.begin())[1];
	mf.classname = (*bqr.begin())[2];

	// perform additional queries for the name of the _max resp _min
	// fieldname
	bqr = qp("select count(*) from mfield where name = '" + mf.name
		+ "_max'");
	mf.hasmax = (1 == atoi((*bqr.begin())[0].c_str()));
	bqr = qp("select count(*) from mfield where name = '" + mf.name
		+ "_min'");
	mf.hasmin = (1 == atoi((*bqr.begin())[0].c_str()));

	// store the mf structure in the cache
	mfieldmap.insert(mfieldmap_t::value_type(mfieldid, mf));
	fieldidmap.insert(fieldidmap_t::value_type(mf.name, mfieldid));

	// report on completion
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "lookup(%d) complete", mfieldid);
	return mfieldid;
}

// lookup based on fully qualified fieldname
int	Field_internals::lookup(const std::string& fieldname) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "lookup for %s", fieldname.c_str());

	// check whether the fqfn appears in the fieldid map, in which
	// case we just return
	if (fieldidmap.find(fieldname) != fieldidmap.end()) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "cache hit for %s",
			fieldname.c_str());
		return fieldidmap.find(fieldname)->second;
	}

	// perform a query to retrieve id, unit, class
	std::string	query
		= "select id from mfield where name = '" + fieldname + "'";
	BasicQueryResult	bqr = qp(query);
	if (bqr.size() == 0) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "nothing known about %s",
			fieldname.c_str());
		throw MeteoException("nothing known about field", fieldname);
	}
	int	id = atoi((*bqr.begin())[0].c_str());
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "found id %d for %s", id,
		fieldname.c_str());

	// perform a lookup for the id, which will complete the mfieldmap data
	return lookup(id);
}

// the two basic accessors both return an mfield_t, the both work by
// first calling lookup, which fills the cache, and then returning
// the appropriate data from the cache
mfield_t	Field_internals::getField(const int mfieldid) {
	lookup(mfieldid);
	return mfieldmap.find(mfieldid)->second;
}
mfield_t	Field_internals::getField(const std::string& fieldname) {
	int	mfieldid = lookup(fieldname);
	return mfieldmap.find(mfieldid)->second;
}

// all other accessor functions are based on the cache maintained by the
// two lookup functions

// retrieve mfield id
int	Field_internals::getId(const std::string& fieldname) {
	// look for the name
	return lookup(fieldname);
}

std::string	Field_internals::getName(const int mfieldid) {
	// perform a lookup, this will add the info to the cache
	return getField(mfieldid).name;
}

} /* namespace meteo */
