/*
 * Outlet.h -- abstract output channel for data
 *
 * (c) 2008 Prof. Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: Outlet.h,v 1.2 2009/01/10 21:47:01 afm Exp $
 */
#ifndef _Outlet_h
#define _Outlet_h

#include <time.h>
#include <list>
#include <string>

namespace meteo {

typedef struct outlet_s {
	int	sensorid;
	int	fieldid;
	float	value;
	std::string	unit;
} outlet_t;

class Outlet {
protected:
	std::list<outlet_t>	batch;
public:
	Outlet();
	virtual	~Outlet();
	virtual void	send(const int sensorid, const int fieldid, const float value, const std::string& unit);
	virtual	void	flush(const time_t timekey);
};

} /* namespace meteo */

#endif /* _Outlet_h */
