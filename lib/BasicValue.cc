/*
 * BasicValue.cc -- implements the BasicValue class, which is intended to
 *                  be a common subclass of all the value types.
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: BasicValue.cc,v 1.9 2009/01/10 19:00:23 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <BasicValue.h>
#include <mdebug.h>
#include <MeteoException.h>

namespace meteo {

// constructors and destructors
BasicValue::BasicValue(const std::string& u) : unit(u) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "create basic value %p (%s)", this,
		u.c_str());
	refcount = 0;
	hasvalue = false;
}

BasicValue::~BasicValue(void) {
	if  (refcount != 0) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "refcount not zero");
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "destroy basic value %p", this);
}

// the following operators should never be called, so we throw an excpetion
// if this happens
BasicValue::BasicValue(const BasicValue& other) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "create basic value %p (copy)", this);
	hasvalue = other.hasvalue;
	unit = other.unit;
	refcount = 0;
	value = other.value;
}
BasicValue&	BasicValue::operator=(const BasicValue& other) {
	hasvalue = other.hasvalue;
	unit = other.unit;
	refcount = 0;
	value = other.value;
	return *this;
}

// member functions
void	BasicValue::setValue(double v) {
	value = v;
	hasvalue = true;
}

std::string	BasicValue::getValueString(const std::string& format) const {
	char	buffer[32];
	if (!hasvalue) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "NULL value found!");
		return std::string("NULL");
	}
	snprintf(buffer, sizeof(buffer), format.c_str(), value);
	return std::string(buffer);
}

std::string	BasicValue::getValueString(void) const {
	return getValueString("%.2f");
}

double	BasicValue::getValue(void) const {
	if (!hasvalue) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "no value found");
		throw MeteoException("no value found", "");
	}
	return value;
}

std::string	BasicValue::plain(const std::string& name) const {
	return name + " " + getValueString() + " " + getUnit();
}

std::string	BasicValue::xml(const std::string& name) const {
	return "<value unit=\"" + getUnit() + "\" name=\"" + name + "\">"
		+ getValueString() + "</value>";
}

} /* namespace meteo */
