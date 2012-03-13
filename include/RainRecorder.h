/* 
 * RainRecorder.h -- encapsulate how rain is measured
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung 
 *
 * $Id: RainRecorder.h,v 1.6 2006/05/16 11:19:54 afm Exp $
 */
#ifndef _RainRecorder_h
#define _RainRecorder_h

#include <Timeval.h>
#include <mdebug.h>
#include <BasicRecorder.h>
#include <Rain.h>

namespace meteo {

class	RainRecorder : public BasicRecorder {
	double		startvalue;
public:
	RainRecorder(void);
	RainRecorder(double v);
	RainRecorder(double v, const std::string& u);
	RainRecorder(const std::string& u);
	~RainRecorder(void);

	std::string	getValueClass(void) const { return "Rain"; }

	// primitive accessors
	virtual void	setValue(double v) {
		startvalue = 0; BasicRecorder::setValue(v);
	}
	void	setTotal(double v) {
		double d = value - startvalue;
		value = v; startvalue = value - d;
	}

	// override the update(double) method of BasicRecorder
	virtual void	update(double rain);

	// override the reset method
	virtual void	reset(void);

	// update query
	virtual stringlist	updatequery(const time_t timekey,
		const int stationid, const int fieldid) const;

	// access data
	virtual double	getValue(void) const;
	double	getTotal(void) const;
	double	getRate(void) const;

	// get string form
	virtual std::string	getValueString(void) const;
	std::string	getTotalString(void) const;

	// return data
	virtual std::string	xml(const std::string& name) const;
	virtual std::string	plain(void) const;

	friend class RainRecorderConverter;
};

} /* namespace meteo */

#endif /* _RainRecorder_h */
