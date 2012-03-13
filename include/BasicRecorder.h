 /*
 * BasicRecorder.h -- base class for all the recorder classes. Recorder classes
 *                    take care of accumulating data, but the cannot be
 *                    converted to other units. They also know about maxima
 *                    and minima (at least some derived classes do), but
 *                    there is no direct relation to Recorder classes, which are
 *                    only used for updating)
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _BasicRecorder_h
#define _BasicRecorder_h

#include <Timeval.h>
#include <MeteoTypes.h>
#include <Converter.h>


namespace meteo {

class BasicRecorder {
	bool		isvalid;
	int		refcount;
	BasicRecorder(const BasicRecorder&);
	BasicRecorder&	operator=(const BasicRecorder&);
protected:
	Converter	conv;
	Timeval		start;
	Timeval		lastupdate;
	double		value;
	virtual void	update(double v);
public:
	BasicRecorder(const std::string& unit);
	virtual	~BasicRecorder(void);
	virtual void	reset(void);	// resets data fileds
	virtual std::string	getValueClass(void) const {
		return "BasicValue";
	}

	// accessors to the value field
	double	getValue(void) const;
	std::string	getValueString(const std::string& format) const;
	virtual std::string	getValueString(void) const;
	virtual void	setValue(double v);

	// public updates should go through a Value reference, which performs
	// type and unit checking (and may be overridden in some special
	// derived classes, notably Wind and Rain)
	virtual void	update(const Value& v);

	// knows how to update the value field, this may be overridden by
	// subclasses
	virtual stringlist updatequery(const time_t timekey,
		const int sensorid, const int fieldid) const;

	// unit accessors
	const std::string&	getUnit(void) const { return conv.getUnit(); }
	
	// isvalid accessor
	bool	isValid(void) const { return isvalid; }
	void	setValid(bool h) { isvalid = h; }

	// output
	virtual std::string	plain(void) const;
	virtual std::string	xml(void) const;

	// allow access to the data fields to the Converter class
	friend class	Recorder;	// may update the reference count
};

} /* namespace meteo */

#endif /* _BasicRecorder_h */
