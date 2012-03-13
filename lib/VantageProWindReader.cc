/*
 * VantageProWindReader.cc -- read wind off a Davis VantagePro loop packet
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: VantageProWindReader.cc,v 1.5 2009/01/10 19:00:25 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <VantageProWindReader.h>
#include <MeteoException.h>
#include <mdebug.h>
#include <ValueFactory.h>

namespace meteo {

VantageProWindReader::VantageProWindReader(void)
		: windspeedreader(14, false), winddirreader(16, false, true) {
}

VantageProWindReader::~VantageProWindReader(void) {
}

double	VantageProWindReader::value(const std::string& packet) const {
	if (!valid(packet)) {
		throw MeteoException("invalid wind data in packet", "");
	}
	return windspeedreader(packet);
}

Value	VantageProWindReader::v(const std::string& packet) const {
	if (!valid(packet)) {
		throw MeteoException("invalid wind data in packet", "");
	}
	double	speed = windspeedreader(packet);
	double	azideg = winddirreader(packet);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "wind azi %.f, speed %.1f",
		azideg, speed);

	ValueFactory	vf;
	return vf.getValue(Vector(speed, azideg, true), "mph");	
}

bool	VantageProWindReader::valid(const std::string& packet) const {
	return (winddirreader.valid(packet) && windspeedreader.valid(packet));
}

} /* namespace meteo */
