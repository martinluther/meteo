/*
 * MsgDequeuer.h -- class that takes a query and sends it to a queue
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _MsgDequeuer_h
#define _MsgDequeuer_h

#include <MsgCommon.h>

namespace meteo {

class MsgDequeuer : public MsgCommon {
public:
	MsgDequeuer(const std::string& name);
	~MsgDequeuer(void);
	std::string	operator()(void);
};

} /* namespace meteo */

#endif /* _MsgDequeuer_h */
