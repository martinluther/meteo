/*
 * WindRecorder.h -- wind class
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _WindRecorder_h
#define _WindRecorder_h

#include <MeteoTypes.h>
#include <Vector.h>
#include <Wind.h>
#include <BasicRecorder.h>
#include <Timeval.h>

namespace meteo {

class	WindRecorder : public BasicRecorder {
	// current/average value
	Vector	v;
	// maximum value
	Vector	max;
	// number of samples
	int	samples;
public:
	// construction and destruction
	WindRecorder(void);
	WindRecorder(const Vector& v0);
	WindRecorder(const Vector& v0, const std::string& u);
	WindRecorder(const std::string& u);
	~WindRecorder(void);

	// value class accessor
	virtual std::string	getValueClass(void) const { return "Wind"; }

	// updaters
	virtual void	reset(void);	// overrides BasicRecorder
	virtual void	update(const Value& w);	// overrides BasicRecorder
	void	update(const Vector& v);

	// accessors
	double	getValue(void) const { return v.getAbs(); }
	double	getAzi(void) const { return v.getAzi(); }
	double	getAzideg(void) const { return v.getAzideg(); }
	double	getMax(void) const { return max.getAbs(); }
	double	getMaxAzi(void) const { return max.getAzi(); }
	double	getMaxAzideg(void) const { return max.getAzideg(); }
	double	getDuration(void) const;
	void	setValue(const Vector& vv) { v = vv; setValid(true); }
	void	setMax(const Vector& vv) { max = vv; }

	// string representation (suitable for SQL queries)
	std::string	getSpeedString(void) const;
	std::string	getXString(void) const;
	std::string	getYString(void) const;
	std::string	getAziString(void) const;	// always degrees
	std::string	getMaxString(void) const;
	std::string	getDurationString(void) const;

	// output
	virtual std::string	plain(void) const;
	virtual std::string	xml(void) const;

	// update query
	virtual stringlist	updatequery(const time_t timekey,
		const int stationid, const int fieldid) const;
};

} // namespace meteo

#endif /* _WindRecorder_h */
