/*
 * FileChannel.h -- abstract communication device to talk to the station
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: FileChannel.h,v 1.2 2004/02/25 23:52:34 afm Exp $
 */
#ifndef _FileChannel_h
#define _FileChannel_h

#include <Channel.h>
#include <string>

namespace meteo {

class FileChannel : public Channel {
public:
	FileChannel(void);
	FileChannel(const std::string& filename);
	virtual	~FileChannel(void);
	virtual void	sendChar(unsigned char c);
	virtual void	sendString(const std::string& s);
};

} /* namespace meteo */

#endif /* _FileChannel_h */

