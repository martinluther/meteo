/*
 * BasicRecorder.cc -- implements the BasicRecorder class, which is intended to
 *                     be a common subclass of all the Recorder types.
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: BasicRecorder.cc,v 1.11 2006/05/16 11:19:54 afm Exp $
 */
#include <BasicRecorder.h>
#include <mdebug.h>
#include <MeteoException.h>

namespace meteo {

// the BasicRecorder always remembers the unit it was created for, and needs
// some member values reset to well defined, nondefault values
BasicRecorder::BasicRecorder(const std::string& unit) : conv(unit) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "creating BasicRecorder %p %s", this,
		unit.c_str());
	refcount = 0;
	reset();
}

// there is nothing to destroy
BasicRecorder::~BasicRecorder(void) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "destroying BasicRecorder %p", this);
}

// updating from a double should really only be used by derived classes,
// the 
void	BasicRecorder::update(double v) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "updating with value %.2f", v);
	if (isvalid) {
		double	t1 = (lastupdate - start).getValue();
		Timeval	s;
		s.now();
		double	t2 = (s - lastupdate).getValue();
		value = (t1 * value + t2 * v)/(t1 + t2);
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "new average: %.2f", value);
		lastupdate.now();
	} else {
		value = v;
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "new value: %.2f", value);
		lastupdate.now();
		isvalid = true;
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "update complete");
}

// update from a Value
void	BasicRecorder::update(const Value& v) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "value class update method called");
	// first check value compatibility
	if (this->getValueClass() != v.getClass()) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "update from different Value "
			"class undefined: %s != %s",
			this->getValueClass().c_str(), v.getClass().c_str());
		throw MeteoException("update from wrong value class",
			v.getClass());
	}

	// convert the value to the unit of the present Recorder
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "convert to target value %s -> %s",
		v.getUnit().c_str(), conv.getUnit().c_str());
	Value	updatevalue = conv(v);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "converted value: %f",
		updatevalue.getValue());

	// use the private update method to update based (this may be virtual
	// again, so we have to go through the this pointer)
	this->update(updatevalue.getValue());
}

void	BasicRecorder::setValue(double v) {
	start.now(); lastupdate.now();
	value = v;
	isvalid = true;
}

std::string	BasicRecorder::getValueString(const std::string& format) const {
	char	buffer[32];
	if (!isvalid) {
		return std::string("NULL");
	}
	snprintf(buffer, sizeof(buffer), format.c_str(), value);
	return std::string(buffer);
}

std::string	BasicRecorder::getValueString(void) const {
	return getValueString("%.2f");
}

double	BasicRecorder::getValue(void) const {
	if (!isvalid)
		throw MeteoException("no value found", "");
	return value;
}

void	BasicRecorder::reset(void) {
	isvalid = false;
	value = 0.0;
	start.now();
}

// the updatequery method returns an insert query for the value, this method
// needs overriding for values that have a minimum and a maximum (where
// additional queries are necessary)
stringlist	BasicRecorder::updatequery(const time_t timekey,
		const int sensorid, const int fieldid) const {

	char	query[1024];
	snprintf(query, sizeof(query),
		"insert into sdata(timekey, sensorid, fieldid, value) "
		"values(%ld, %d, %d, %.5f)",
		timekey, sensorid, fieldid, value);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "udpate query: %s", query);
	
	stringlist	result;
	result.push_back(std::string(query));
	return result;
}

std::string	BasicRecorder::plain(void) const {
	return getValueString() + " " + getUnit();
}

std::string	BasicRecorder::xml(const std::string& name) const {
	return "<data name=\"" + name
		+ "\" value=\"" + this->getValueString()
		+ "\" unit=\"" + conv.getUnit()
		+ "/>";
}

} /* namespace meteo */
