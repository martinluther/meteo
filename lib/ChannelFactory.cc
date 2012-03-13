/*
 * ChannelFactory.cc -- encapsulate the communication logic 
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung 
 *
 * $Id: ChannelFactory.cc,v 1.2 2004/02/25 23:48:04 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <ChannelFactory.h>
#include <TcpChannel.h>
#include <SerialChannel.h>
#include <FileChannel.h>
#include <Configuration.h>
#include <strings.h>
#include <MeteoException.h>
#include <mdebug.h>

namespace meteo {

Channel	*ChannelFactory::newChannel(const std::string& name) {
	Configuration	conf;
	std::string	url = conf.getString("/meteo/station[@name='"
		+ name + "']/url", "undefined");

	// find out what type of a device the url describes
	Channel	*result = NULL;
	if (url.substr(0, 4) == "tcp:") {
		// TCP case
		result	= new TcpChannel(url);
	} else if (url.substr(0, 5) == "file:") {
		// try to guess the baud rate from the type
		std::string	type = conf.getString("/meteo/station[@name='"
			+ name + "']/type", "");
		int	baudrate;
		if (type == "WMII") {
			baudrate = 2400;
		} else if (type == "VantagePro") {
			baudrate = 19200;
		} else {
			baudrate = 9600;
		}
		baudrate = conf.getInt("/meteo/station[@name='"
			+ name + "']/speed", baudrate);

		// get the device name
		std::string	device = url.substr(7);
		result = new SerialChannel(device, baudrate);
	} else if (url.substr(0, 6) == "image:") {
		std::string	file = url.substr(8);
		result = new FileChannel(file);
	} else {
		throw MeteoException("unknown URL type", url);
	}
	return result;
}

} /* namespace meteo */
