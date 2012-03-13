/*
 * BasicPacketReader.cc -- class that abstracts reading numeric data
 *                         from a data packet
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: BasicPacketReader.cc,v 1.10 2009/01/10 19:00:23 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <BasicPacketReader.h>
#include <ValueFactory.h>
#include <mdebug.h>
#include <MeteoException.h>

namespace meteo {

BasicPacketReader::BasicPacketReader(void) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "create BasicPacketReader %p", this);
	refcount = 0;
	offset = 0;
}

BasicPacketReader::BasicPacketReader(int o) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "create BasicPacketReader %p", this);
	refcount = 0;
	offset = o;
}

BasicPacketReader::~BasicPacketReader(void) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "destroy BasicPacketReader %p", this);
}

BasicPacketReader::BasicPacketReader(const BasicPacketReader& other) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "create BasicPacketReader %p (copy)",
		this);
	throw MeteoException("cannot copy BasicPacketReader", "");
}
BasicPacketReader&	BasicPacketReader::operator=(const BasicPacketReader& other) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "create BasicPacketReader %p (assign)",
		this);
	throw MeteoException("cannot assign BasicPacketReader", "");
	return *this;
}

void	BasicPacketReader::calibrate(const Calibrator& c) {
	cal = c;
}

double	BasicPacketReader::operator()(const std::string& packet) const {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "reading calibrated %s value",
		classname.c_str());
	double	vv = this->value(packet);
	double	result = cal(this->value(packet));
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "uncalibrated: %f, calibrated: %f",
		vv, result);
	return result;
}

Value	BasicPacketReader::v(const std::string& packet) const {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "building %s (%s)", classname.c_str(),
		unit.c_str());
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "BasicPacketReader::v");
	double	f = this->operator()(packet);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "value %f", f);
	ValueFactory	vf;
	return vf.getValue(classname, f, unit);
}

} /* namespace meteo */

