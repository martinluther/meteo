/*
 * MoistureRecorder.h -- soil moisture abstraction
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _MoistureRecorder_h
#define _MoistureRecorder_h

#include <MinmaxRecorder.h>
#include <string>

namespace meteo {

class	MoistureRecorder : public MinmaxRecorder {
public:
	MoistureRecorder(void);
	MoistureRecorder(double);
	MoistureRecorder(double, const std::string&);
	MoistureRecorder(const std::string&);
	virtual	~MoistureRecorder(void) { }
	virtual std::string	getValueClass(void) const {
		return "MoistureValue";
	}
};

} // namespace meteo

#endif /* _MoistureRecorder_h */
