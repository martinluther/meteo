/*
 * MinmaxRecorder.cc -- handle meteo value including maximum and minimum and
 *                   aggregation
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#include <MinmaxRecorder.h>
#include <MeteoException.h>
#include <mdebug.h>

namespace meteo {

// constructors and destructors
MinmaxRecorder::MinmaxRecorder(const std::string& unit) : BasicRecorder(unit) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "creating MinmaxRecorder %s",
		unit.c_str());
	reset();
	storeminmax = false;
}
MinmaxRecorder::~MinmaxRecorder(void) {
}

// update the value, this must now also take into account the minimum and
// maximum value
void	MinmaxRecorder::update(double v) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "updating with value %.2f", v);
	if (isValid()) {
		// update the maximum
		if (v > max)
			max = v;

		// update the minimum
		if (v < min)
			min = v;
	} else {
		min = max = v;
	}
	BasicRecorder::update(v);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "update complete");
}

// set the value, this resets time stamps and max/min
void	MinmaxRecorder::setValue(double v) {
	max = v;
	min = v;
	BasicRecorder::setValue(v);
}

std::string	MinmaxRecorder::getMinString(void) const {
	char	buffer[32];
	if (!isValid())
		return std::string("NULL");
	snprintf(buffer, sizeof(buffer), "%.2f", min);
	return std::string(buffer);
}

std::string	MinmaxRecorder::getMaxString(void) const {
	char	buffer[32];
	if (!isValid())
		return std::string("NULL");
	snprintf(buffer, sizeof(buffer), "%.2f", max);
	return std::string(buffer);
}

double	MinmaxRecorder::getMax(void) const {
	if (!isValid())
		throw MeteoException("no max value", "");
	return value;
}

double	MinmaxRecorder::getMin(void) const {
	if (!isValid())
		throw MeteoException("no min value", "");
	return min;
}

void	MinmaxRecorder::reset(void) {
	max = -1000000.;
	min =  1000000.;
	BasicRecorder::reset();
}

stringlist	MinmaxRecorder::updatequery(const time_t timekey,
	const int sensorid, const int fieldid) const {
	stringlist	result;
	// return empty result if no value available
	if (!isValid())
		return result;

	// get the BasicRecorder's update query
	result = BasicRecorder::updatequery(timekey, sensorid, fieldid);

	// get maximum and minimu update query
	if (storeminmax) {
		char	query[1024];
		snprintf(query, sizeof(query),
			"insert into sdata(timekey, sensorid, fieldid, value) "
			"values (%ld, %d, %d, %.5f)",
			timekey, sensorid, fieldid + 1, min);
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "min query: %s", query);
		result.push_back(std::string(query));
		snprintf(query, sizeof(query),
			"insert into sdata(timekey, sensorid, fieldid, value) "
			"values (%ld, %d, %d, %.5f)",
			timekey, sensorid, fieldid + 2, max);
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "max query: %s", query);
		result.push_back(std::string(query));
	}

	// return the queries
	return result;
}

std::string	MinmaxRecorder::plain(void) const {
	char	result[1024];
	snprintf(result, sizeof(result),
		"%5.1f %s (min: %5.1f, max: %5.1f)",
		value, getUnit().c_str(), min, max);
	return result;
}
std::string	MinmaxRecorder::xml(void) const {
	return	"<value>" + getValueString() + "</value>"
		+ "<min>" + getMinString() + "</min>"
		+ "<max>" + getMaxString() + "</max>";
}

} /* namespace meteo */
