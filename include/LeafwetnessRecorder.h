/*
 * LeafwetnessRecorder.h -- Leaf wetness abstraction
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _LeafwetnessRecorder_h
#define _LeafwetnessRecorder_h

#include <MinmaxRecorder.h>
#include <string>

namespace meteo {

class	LeafwetnessRecorder : public MinmaxRecorder {
public:
	LeafwetnessRecorder(void);
	LeafwetnessRecorder(double);
	LeafwetnessRecorder(double, const std::string&);
	LeafwetnessRecorder(const std::string&);
	virtual	~LeafwetnessRecorder(void) { }
	virtual std::string	getValueClass(void) const {
		return "LeafwetnessValue";
	}
};

} // namespace meteo

#endif /* _LeafwetnessRecorder_h */
