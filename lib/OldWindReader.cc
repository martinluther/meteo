/*
 * OldWindReader.cc -- read wind off a Davis Weather Monitor loop packet
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: OldWindReader.cc,v 1.5 2009/01/10 19:00:24 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <OldWindReader.h>
#include <ValueFactory.h>

namespace meteo {

OldWindReader::OldWindReader(int o) : winddirreader(o + 1, false, 3),
					windspeedreader(o, false) {
}

OldWindReader::~OldWindReader(void) {
}

double	OldWindReader::value(const std::string& packet) const {
	return windspeedreader(packet);
}

Value	OldWindReader::v(const std::string& packet) const {
	double	speed = windspeedreader.value(packet);
	double	azideg = winddirreader.value(packet);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "wind azi %.f, speed %.1f",
		azideg, speed);

	ValueFactory	vf;
	return vf.getValue(Vector(speed, (short unsigned int)azideg, true),
		"mph");	
}

bool	OldWindReader::valid(const std::string& packet) const {
	return (winddirreader.valid(packet) && windspeedreader.valid(packet));
}

} /* namespace meteo */
