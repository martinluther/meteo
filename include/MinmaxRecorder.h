/*
 * MinmaxRecorder.h -- base class for meteorological values retrieved from
 *                  a weather station, allows for updates and knows about
 *                  about maximum and minimum
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: MinmaxRecorder.h,v 1.7 2008/09/07 15:18:52 afm Exp $
 */
#ifndef _MinmaxRecorder_h
#define _MinmaxRecorder_h

#include <BasicRecorder.h>
#include <Timeval.h>
#include <Outlet.h>

namespace meteo {

class Converter;

class	MinmaxRecorder : public BasicRecorder {

protected:
	// maximum value
	double	max;

	// minimum value
	double	min;

	// decide whether min/max should go to the database
	bool	storeminmax;
public:
	// construction and destruction
	MinmaxRecorder(const std::string& unit);
	virtual ~MinmaxRecorder(void);

	// update the meteovalue with a new value
	void	reset(void);
	virtual void	update(double v);
	virtual void	setValue(double v);

	// update query
	virtual void	sendOutlet(Outlet *outlet, const time_t timekey,
		const int stationid, const int fieldid) const;

	// retrieve the values
	double	getMax(void) const;
	double	getMin(void) const;

	// retrieve a std::string form (suitable for SQL queries)
	std::string	getMinString(void) const;
	std::string	getMaxString(void) const;

	// connect
	virtual std::string	plain(void) const;
	virtual std::string	xml(const std::string& name) const;
};

} // namespace meteo

#endif /* _MinmaxRecorder_h */
