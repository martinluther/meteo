/*
 * Query.cc -- perform database queries against 
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: Query.cc,v 1.19 2009/01/10 19:00:24 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <Query.h>
#include <mdebug.h>
#include <MeteoException.h>
#include <Field.h>
#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#endif /* HAVE_ALLOCA_H */

namespace meteo {

// the addSelect method maintains the select and idmap maps, in particular,
// the 
void	Query::addSelect(const std::string& n, const std::string& fieldname) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "--> addSelect(%s, %s)", n.c_str(),
		fieldname.c_str());
	// first check that the symbolic name has not been used yet
	if (select.find(n) != select.end()) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "duplicate data name: %s",
			n.c_str());
		throw MeteoException("duplicate data name", n);
	}

	// now retrieve the fieldid for the fully qualified fieldname
	// (this is also a test whether the fieldname does exist)
	fieldid	id = FQField().getFieldid(fieldname);
	if (idmap.find(id) != idmap.end()) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "duplicate id for name: %s",
			n.c_str());
		throw MeteoException("duplicate id for name", n);
	}

	// if we get to this point, there is no reason why we should not
	// be able to insert the info in both maps, so they will stay
	// consistent, out of niceness we log what we are storing
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "addSelect store %s -> %s (%d,%d,%d)",
		n.c_str(), fieldname.c_str(), id.stationid, id.sensorid,
		id.mfieldid);

	// add the relation n->fqfieldname to the select map
	select.insert(smap_t::value_type(n, fieldname));

	// add the inverse map to the idmap
	idmap.insert(imap_t::value_type(id, n));
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "<-- addSelect(%s, %s)", n.c_str(),
		fieldname.c_str());
}

std::string	Query::getQuery(void) const {
	// no reasonable where clause can be built if no fields are requested
	if (select.size() == 0) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "no fields selected");
		throw MeteoException("no fields selected", "");
	}

	// build the basic part of the query
	char	b[1024];
	if (interval != 60) {
		snprintf(b, sizeof(b),
			"select timekey, sensorid, fieldid, value "
			"from avg "
			"where timekey >= %ld "
			"  and timekey <= %ld "
			"  and intval = %d ",
			start, end, interval);
	} else {
		snprintf(b, sizeof(b),
			"select timekey + %d, sensorid, fieldid, value from sdata "
			"where timekey >= %ld "
			"  and timekey <= %ld ",
			offset, start, end);
	}
	std::string	query(b);

	// create an in clause for the field ids we need
	std::string	clause;
	smap_t::const_iterator	i;
	for (i = select.begin(); i != select.end(); i++) {
		// if we are the first entry, add an opening for the large
		// and clause that follows
		if (i == select.begin()) {
			query.append(" and (");
		} else {
			query.append(" or ");
		}

		// the second component is the field name
		fieldid fi = FQField().getFieldid(i->second);
		char	idstr[128];
		snprintf(idstr, sizeof(idstr),
			" (sensorid = %d and fieldid = %d) ",
			fi.sensorid, fi.mfieldid);
		query.append(idstr);
	}
	query.append(")");

	// return a std::string containing the query
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "query is '%s'", query.c_str());
	return query;
}

} /* namespace meteo */
