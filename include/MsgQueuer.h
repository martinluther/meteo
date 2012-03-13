/*
 * MsgQueuer.h -- class that takes a query and sends it to a queue
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _MsgQueuer_h
#define _MsgQueuer_h

#include <MsgCommon.h>

namespace meteo {

class MsgQueuer : public MsgCommon {
public:
	MsgQueuer(const std::string& name);
	~MsgQueuer(void);
	void	operator()(const std::string& query);
};

} /* namespace meteo */

#endif /* _MsgQueuer_h */
