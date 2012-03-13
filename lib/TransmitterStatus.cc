/*
 * TransmitterStatus.cc -- implement voltage (for VantagePro console battery)
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: TransmitterStatus.cc,v 1.2 2004/02/25 23:48:06 afm Exp $
 */
#include <TransmitterStatus.h>
#include <MeteoException.h>
#include <VoltageConverter.h>
#include <mdebug.h>

namespace meteo {

// TransmitterStatus methods
TransmitterStatus::TransmitterStatus(void) : BasicValue("") {
}
TransmitterStatus::TransmitterStatus(double v) : BasicValue("") {
	setValue(v);
}

TransmitterStatus::TransmitterStatus(double v, const std::string& u)
	: BasicValue(u) {
	setValue(v);
}

TransmitterStatus::TransmitterStatus(const std::string& u) : BasicValue(u) {
}

void	TransmitterStatus::setUnit(const std::string& targetunit) {
}

std::string	TransmitterStatus::plain(const std::string& name) const {
	std::string	result;
	int	transmitterbits = (int)getValue();
	for (int mask = 128; mask > 0; mask >>= 1) {
		result = ((mask & transmitterbits) ? std::string("+ ")
						   : std::string("- "))
			+ result;
	}
	return result;
}

std::string	TransmitterStatus::xml(const std::string& name) const {
	std::string	result;
	int	transmitterbits = (int)getValue();
	for (int mask = 128, i = 8; mask > 0; mask >>= 1, i--) {
		char	buf[128];
		snprintf(buf, sizeof(buf), "<xmit id=\"%d\">%d</xmit>",
			i, (transmitterbits & mask) ? 1 : 0);
		result = std::string(buf) + result;
	}
	return result;
}

} /* namespace meteo */
