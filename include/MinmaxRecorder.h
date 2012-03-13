/*
 * MinmaxRecorder.h -- base class for meteorological values retrieved from
 *                  a weather station, allows for updates and knows about
 *                  about maximum and minimum
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: MinmaxRecorder.h,v 1.5 2004/02/25 23:52:34 afm Exp $
 */
#ifndef _MinmaxRecorder_h
#define _MinmaxRecorder_h

#include <BasicRecorder.h>
#include <Timeval.h>

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
	virtual stringlist	updatequery(const time_t timekey,
		const int stationid, const int fieldid) const;

	// retrieve the values
	double	getMax(void) const;
	double	getMin(void) const;

	// retrieve a std::string form (suitable for SQL queries)
	std::string	getMinString(void) const;
	std::string	getMaxString(void) const;

	// connect
	virtual std::string	plain(void) const;
	virtual std::string	xml(void) const;
};

} // namespace meteo

#endif /* _MinmaxRecorder_h */
