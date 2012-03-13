/*
 * QueryOutlet.cc -- outlet subclass that sends queries
 *
 * (c) 2008 Prof Dr Andreas Mueller
 * $Id: QueryOutlet.cc,v 1.2 2009/01/10 19:00:24 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <QueryOutlet.h>
#include <list>
#include <Datasink.h>
#include <Format.h>

namespace meteo {

QueryOutlet::QueryOutlet(const std::string& stationname) {
	ds = DatasinkFactory().newDatasink(stationname);
}

QueryOutlet::~QueryOutlet() {
	if (NULL != ds) {
		delete ds;
	}
}

void	QueryOutlet::flush(const time_t timekey) {
	// construct a list of queries
	std::list<outlet_t>::iterator	i;
	stringlist	queries;
	for (i = batch.begin(); i != batch.end(); i++) {
		std::string	query = stringprintf(
			"insert into sdata (timekey, sensorid, fieldid, value) "
			"values(%ld, %d, %d, %.7f)",
			timekey, i->sensorid, i->fieldid, i->value);
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "query: %s", query.c_str());
		queries.push_back(query);
	}

	// send the queries
	ds->receive(queries);

	// do whatever the base class wishes to do
	Outlet::flush(timekey);
}

} // namespace meteo
