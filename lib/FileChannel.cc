/*
 * FileChannel.cc -- encapsulate the communication logic 
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung 
 *
 * $Id: FileChannel.cc,v 1.2 2004/02/25 23:48:05 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <FileChannel.h>
#include <Configuration.h>
#include <strings.h>
#include <errno.h>
#include <MeteoException.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#include <Timeval.h>
#include <mdebug.h>

namespace meteo {

FileChannel::FileChannel(void) {
	// use standard input
	f = fileno(stdin);
}
FileChannel::FileChannel(const std::string& filename) {
	if ((f = open(filename.c_str(), O_RDONLY)) < 0) {
		// throw an IO exception
		throw MeteoException("cannot open file", filename);
	}
}
FileChannel::~FileChannel(void) { }

void	FileChannel::sendChar(unsigned char c) { }
void	FileChannel::sendString(const std::string& s) { }

} /* namespace meteo */
