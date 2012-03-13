/*
 * Field.cc -- implement field query oracle
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung 
 *
 * $Id: Field.cc,v 1.10 2004/02/25 23:48:05 afm Exp $
 */
#include <Field.h>
#include <Field_internals.h>
#include <mdebug.h>

namespace meteo {

// internals: this implements a cache for field information
Field_internals	*Field::fi	= NULL;


// Field  class implementation, mainly delegation
Field::Field(void) {
	if (NULL == fi) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "initializing Field::fi");
		fi = new Field_internals();
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "MM Field create %p", this);
}
Field::~Field(void) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "MM Field destroy %p", this);
}

int	Field::getId(const std::string& fieldname) {
	return fi->getId(fieldname);
}

std::string	Field::getName(const int mfieldid) {
	return fi->getName(mfieldid);
}

// maximum and minimum
bool	Field::hasMaximum(const int mfieldid) {
	return fi->getField(mfieldid).hasmax;
}

bool	Field::hasMinimum(const int mfieldid) {
	return fi->getField(mfieldid).hasmin;
}

bool	Field::hasMaximum(const std::string& fieldname) {
	return fi->getField(fieldname).hasmax;
}

bool	Field::hasMinimum(const std::string& fieldname) {
	return fi->getField(fieldname).hasmin;
}

bool	Field::isWind(const int mfieldid) {
	return fi->getField(mfieldid).name == "wind";
}

bool	Field::isRain(const int mfieldid) {
	return fi->getField(mfieldid).name == "rain";
}

bool	Field::isWind(const std::string& fieldname) {
	return fi->getField(fieldname).name == "wind";
}

bool	Field::isRain(const std::string& fieldname) {
	return fi->getField(fieldname).name == "rain";
}

// class and unit
std::string	Field::getClass(const int mfieldid) {
	return fi->getField(mfieldid).classname;
}

std::string	Field::getUnit(const int mfieldid) {
	return fi->getField(mfieldid).unit;
}

std::string	Field::getClass(const std::string& fieldname) {
	return fi->getField(fieldname).classname;
}

std::string	Field::getUnit(const std::string& fieldname) {
	return fi->getField(fieldname).unit;
}

} /* namespace meteo */
