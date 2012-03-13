/*
 * WMIIWindReader.cc -- read wind off a Davis Weather Monitor loop packet
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#include <WMIIWindReader.h>
#include <ValueFactory.h>

namespace meteo {

WMIIWindReader::WMIIWindReader(void) : winddirreader(6, false, true),
					windspeedreader(5, false) {
}

WMIIWindReader::~WMIIWindReader(void) {
}

double	WMIIWindReader::value(const std::string& packet) const {
	return windspeedreader(packet);
}

Value	WMIIWindReader::v(const std::string& packet) const {
	double	speed = windspeedreader.value(packet);
	double	azideg = winddirreader.value(packet);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "wind azi %.f, speed %.1f",
		azideg, speed);

	ValueFactory	vf;
	return vf.getValue(Vector(speed, (int)azideg, true), "mph");	
}

bool	WMIIWindReader::valid(const std::string& packet) const {
	return (winddirreader.valid(packet) && windspeedreader.valid(packet));
}

} /* namespace meteo */
