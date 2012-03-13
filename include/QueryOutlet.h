/*
 * QueryOutlet.h -- outlet subclass that sends data to queries
 *
 * (c) 2008 Prof Dr Andreas Mueller
 */
#ifndef _QueryOutlet_h
#define _QueryOutlet_h

#include <Outlet.h>
#include <Datasink.h>

namespace meteo {

class QueryOutlet : public Outlet {
	Datasink	*ds;
public:
	QueryOutlet(const std::string& stationname);
	virtual	~QueryOutlet();
	virtual void	flush(const time_t timekey);
};

} // namespace meteo

#endif /* _QueryOutlet_h */
