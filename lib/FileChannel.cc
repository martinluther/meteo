/*
 * FileChannel.cc -- encapsulate the communication logic 
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung 
 *
 * $Id: FileChannel.cc,v 1.3 2009/01/10 19:00:24 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <FileChannel.h>
#include <Configuration.h>
#include <strings.h>
#include <errno.h>
#include <MeteoException.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif /* HAVE_FCNTL_H */
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif /* HAVE_SYS_TYPES_H */
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
