/*
 * Value.cc -- reference counted Value class that encapsulates all the
 *             Value classes but still gives us the benefit of resource
 *             management
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: Value.cc,v 1.5 2004/02/25 23:48:06 afm Exp $
 */
#include <Value.h>
#include <MeteoException.h>

namespace meteo {

Value::Value(BasicValue *v) {
	bv = v;
	bv->refcount = 1;
}

Value::Value(void) {
	bv = NULL;
}

Value::Value(const Value& other) {
	if (NULL != other.bv) {
		other.bv->refcount++;
		bv = other.bv;
	}
}

void	Value::setValue(double v) {
	if (NULL == bv) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "no value in Value");
		throw MeteoException("no value in Value", "");
	}
	bv->setValue(v);
}

const std::string	Value::getClass(void) const {
	return bv->getClass();
}

Value&	Value::operator=(const Value& other) {
	if (NULL != other.bv)
		other.bv->refcount++;
	if (NULL != bv)
		if (--bv->refcount == 0)
			delete bv;
	bv = other.bv;
	return *this;
}

Value::~Value(void) {
	// ignore incompletely initialized Values
	if (NULL == bv) return;

	// destroy BasicValue at refcount == 0
	if (!--bv->refcount) {
		delete bv;
		bv = NULL;
	}
}

// most methods are simply delegated to the BasicValue
double	Value::getValue(void) const {
	if (NULL == bv)
		throw MeteoException("no value in Value.getValue", "");
	return bv->getValue();
}

std::string	Value::plain(const std::string& name) const {
	if (NULL == bv)
		throw MeteoException("no value in Value.plain", "");
	return bv->plain(name);
}

std::string	Value::xml(const std::string& name) const {
	if (NULL == bv)
		throw MeteoException("no value in Value.xml", "");
	return bv->xml(name);
}

} /* namespace meteo */
