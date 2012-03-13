/*
 * Tdata.cc -- implementation of operators on time dependent data sets
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#include <Tdata.h>

#include <math.h>
#include <MeteoException.h>
#include <mdebug.h>
#include <Dewpoint.h>
#include <algo.h>

namespace meteo {

void	Tdata::addData(const tdata_t& input) {
	// add all pairs in input if they are between start and end and 
	// have a key that is a multiple of the interval
	tdata_t::const_iterator	i;
	for (time_t t = start; t <= finish; t += interval) {
		i = input.find(t);
		if (i != input.end())
			data[i->first] = i->second;
	}
}

double	Tdata::operator()(time_t t) const {
	// if the Tdata is constant, then we simply return its value
	if (constant)
		return value;
	// find the right time
	tdata_t::const_iterator	i = data.find(t);
	if (i == data.end()) {
		throw MeteoException("time value not found", "");
	}
	return i->second;
}

Tdata	Tdata::apply(double (*func)(double)) const {
	Tdata	result(interval, start, finish);
	tdata_t::const_iterator	i;
	for (i = data.begin(); i != data.end(); i++) {
		try {
			result.data[i->first] = func(i->second);
		} catch (...) { }
	}
	return result;
}

Tdata	apply2(double (*func)(double, double), const Tdata& a, const Tdata& b) {
	tdata_t::const_iterator	i;
	int	count = 0;

	// handle the case where both Tdatas are constants
	if ((a.constant) && (b.constant)) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "applying %p to two constants",
			func);
		return Tdata(func(a.value, b.value));
	}

	// handle case where the first Tdata is constant
	if (a.constant) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "applying %p to const/var "
			"(%d elements)", func, b.data.size());
		Tdata	result(b.getInterval(), b.getStart(), b.getFinish());
		for (i = b.data.begin(); i != b.data.end(); i++) {
			try {
				result.data[i->first]
					= func(a.value, i->second);
				count++;
			} catch (...) { }
		}
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "%d items computed", count);
		return result;
	}

	// handle case where the second Tdata is constant
	if (b.constant) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "applying %p to "
			"var(%d elements)/const", func, a.data.size());
		Tdata	result(a.getInterval(), a.getStart(), a.getFinish());
		for (i = a.data.begin(); i != a.data.end(); i++) {
			try {
				result.data[i->first]
					= func(b.value, i->second);
				count++;
			} catch (...) { }
		}
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "%d items computed", count);
		return result;
	}

	// handle case where both tdatas are nonconstant
	if (a.getInterval() != b.getInterval()) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "apply2'ing incompatible: "
			"%d != %d", a.getInterval(), b.getInterval());
		MeteoException("apply2ing incompatible", "different intervals");
	}

	// compute the intersection of the 
	time_t	commonstart, commonfinish;
	commonstart = (a.getStart() < b.getStart())	? a.getStart()
							: b.getStart();
	commonfinish = (a.getFinish() > b.getFinish())	? a.getFinish()
							: b.getFinish();
	Tdata	result(a.getInterval(), commonstart, commonfinish);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "common range: %d - %d",
		commonstart, commonfinish);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "applying %p to var(%d)/var(%d)",
		func, a.data.size(), b.data.size());
	for (i = a.data.begin(); i != a.data.end(); i++) {
		if (b.data.end() != b.data.find(i->first)) {
			if ((i->first < commonstart)
				|| (i->first > commonfinish))
				continue;
			try {
				result.data[i->first] = func(i->second,
					b.data.find(i->first)->second);
				count++;
			} catch (...) { }
		}
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "%d items computed", count);
	return result;
}

static double	operatorplus(double a, double b) { return a+b; }
static double	operatorminus(double a, double b) { return a-b; }
static double	operatortimes(double a, double b) { return a*b; }
static double	operatorquotient(double a, double b) {
	if (b == 0.)
		throw MeteoException("division by zero", "");
	return a/b;
}
static double	operatormax(double a, double b) { return (a > b) ? a : b; }
static double	operatormin(double a, double b) { return (a < b) ? a : b; }
static double	operatoratan2(double x, double y) { return ::atan2(x, y); }
static double	operatorazi(double x, double y) {
	if ((::fabs(x) + ::fabs(y)) < 0.01)
		throw MeteoException("value too small to compute azi", "");
	double	a = 180. * ::atan2(x, y) / 3.1415926535;
	while (a < 0) a+= 360.;
	return a;
}
static double	operatorhypot(double x, double y) { return ::hypot(x, y); }
static Dewpoint	*dpp = NULL;
static double	dewpoint(double h, double t) {
	if (dpp == NULL)
		dpp = new Dewpoint();
	return dpp->operator()(h, t);
}
static double	operatordewpoint(double h, double t) { return dewpoint(h, t); }

Tdata	operator+(const Tdata& a, const Tdata& b) {
	return apply2(operatorplus, a, b);
}

Tdata	operator-(const Tdata& a, const Tdata& b) {
	return apply2(operatorminus, a, b);
}

Tdata	operator*(const Tdata& a, const Tdata& b) {
	return apply2(operatortimes, a, b);
}

Tdata	operator/(const Tdata& a, const Tdata& b) {
	return apply2(operatorquotient, a, b);
}

Tdata	atan2(const Tdata& x, const Tdata& y) {
	return apply2(operatoratan2, x, y);
}

Tdata	azi(const Tdata& x, const Tdata& y) {
	return apply2(operatorazi, x, y);
}

Tdata	hypot(const Tdata& x, const Tdata& y) {
	return apply2(operatorhypot, x, y);
}

Tdata	dewpoint(const Tdata& humidity, const Tdata& temperature) {
	return apply2(operatordewpoint, humidity, temperature);
}

Tdata	max(const Tdata& a, const Tdata& b) {
	return apply2(operatormax, a, b);
}

Tdata	min(const Tdata& a, const Tdata& b) {
	return apply2(operatormin, a, b);
}

static double	operatorsqr(double a) { return a*a; }
static double	operatorsqrt(double a) { return ::sqrt(a); }
static double	operatorsin(double a) { return ::sin(a); }
static double	operatorcos(double a) { return ::cos(a); }
static double	operatortan(double a) { return ::tan(a); }
static double	limit;
static double	operatorfloor(double a) { return (a < meteo::limit) ? meteo::limit : a; }
static double	operatorceil(double a) { return (a < meteo::limit) ? a : meteo::limit; }

Tdata	Tdata::sqr(void) const {
	return apply(operatorsqr);
}

Tdata	Tdata::sqrt(void) const {
	return apply(operatorsqrt);
}

Tdata	Tdata::sin(void) const {
	return apply(operatorsin);
}

Tdata	Tdata::cos(void) const {
	return apply(operatorcos);
}

Tdata	Tdata::tan(void) const {
	return apply(operatortan);
}

Tdata	Tdata::floor(double l) const {
	meteo::limit = l;
	return apply(operatorfloor);
}

Tdata	Tdata::ceil(double l) const {
	meteo::limit = l;
	return apply(operatorceil);
}

Tdata	Tdata::accumulate(void) const {
	double	acc = 0.;
	Tdata	result(interval, start, finish);

	// add a point for each valid time value
	for (int t = start; t <= finish; t += interval) {
		tdata_t::const_iterator	i = data.find(t);
		if (i != data.end()) {
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
				"accumulating value %d -> %f", t, i->second);
			acc += i->second;
		}
		result.data[t] = acc;
	}

	// return the complete tdata object
	return result;
}

// the limiter class is used to remove elements from a Tdata that have 
// values below a certain limit. This is most useful in the Wind graphs
// where it does not make sense to display azimut/speed if the wind speed
// is below say 0.1m/s.
class	limiter {
	double	limit;
public:
	limiter(double l) { limit = l; }
	bool	operator()(const std::pair<time_t, double>& p) {
		return (p.second < limit);
	}
};

Tdata	Tdata::limit(double l) const {
	Tdata	result(interval, start, finish);
	remove_copy_if(data.begin(), data.end(),
		std::insert_iterator< tdata_t >(result.data,
			result.data.begin()), limiter(l));
	return result;
}

// max/min of a Tdata
bool	TdataCompare(const std::pair<time_t, double>& a,
		const std::pair<time_t, double>& b) {
	return a.second < b.second;
}

double	Tdata::max(void) const {
	tdata_t::const_iterator	i = max_element(data.begin(), data.end(),
		TdataCompare);
	if (i != data.end())
		return i->second;
	return -10000000.;
}
double	Tdata::min(void) const {
	tdata_t::const_iterator	i = min_element(data.begin(), data.end(),
		TdataCompare);
	if (i != data.end())
		return i->second;
	return 10000000.;
}

// compute the average of a Tdata
class	Average {
	long	num;
	double	sum;
public:
	Average(void) {
		num = 0;
		sum = 0.;
	}
	void	operator()(const std::pair<time_t, double>& p) {
		sum += p.second; num++;
	}
	double	avg(void) {
		if (num == 0) {
			mdebug(LOG_ERR, MDEBUG_LOG, 0,
				"no elements to average");
			return 0.;
		}
		return sum/num;
	}
};

double	Tdata::avg(void) const {
	Average	a = for_each(data.begin(), data.end(), Average());
	return a.avg();
}

} /* namespace meteo */
