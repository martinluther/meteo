/*
 * Tdata.h -- time dependent data, uses an STL map
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _Tdata_h
#define _Tdata_h

#include <map>
#include <time.h>
#include <string>

namespace meteo {

typedef	std::map<time_t, double>	tdata_t;

class	Tdata {
	int	interval;
	time_t	start, finish;
	tdata_t	data;
	bool	constant;
	double	value;
public:
	// constructors
	Tdata(void) { interval = 0; start = 0; finish = 0; constant = false; }
	Tdata(double v) { value = v; constant = true; }
	Tdata(int i, time_t s, time_t e) {
		interval = i;
		start = s - (s % interval); finish = e - (e % interval);
		constant = false;
	}
	~Tdata(void) { }

	// accessors for data
	void	addData(const tdata_t& input);
	const tdata_t&	getData(void) const { return data; }
	int	getInterval(void) const { return interval; }
	time_t	getStart(void) const { return start; }
	time_t	getFinish(void) const { return finish; }
	bool	isConstant(void) const { return constant; }

	// access to data items
	double	operator()(time_t t) const;
	double&	operator[](time_t t) { return data[t]; }
	tdata_t::const_iterator	begin() const { return data.begin(); }
	tdata_t::const_iterator	end() const { return data.end(); }
	tdata_t::const_iterator	find(time_t t) const { return data.find(t); }

	// global operations on a Tdata
	double	max(void) const;
	double	min(void) const;
	double	avg(void) const;

	// binary operators
	friend Tdata	operator+(const Tdata& a, const Tdata& b);
	friend Tdata	operator-(const Tdata& a, const Tdata& b);
	friend Tdata	operator*(const Tdata& a, const Tdata& b);
	friend Tdata	operator/(const Tdata& a, const Tdata& b);
	friend Tdata	atan2(const Tdata& x, const Tdata& y);
	friend Tdata	azi(const Tdata& x, const Tdata& y);
	friend Tdata	hypot(const Tdata& a, const Tdata& b);
	friend Tdata	dewpoint(const Tdata& hum, const Tdata& temp);
	friend Tdata	max(const Tdata& a, const Tdata& b);
	friend Tdata	min(const Tdata& a, const Tdata& b);

	// monic operations
	Tdata	sqr(void) const;
	Tdata	sqrt(void) const;
	Tdata	sin(void) const;
	Tdata	cos(void) const;
	Tdata	tan(void) const;
	Tdata	ceil(double limit) const;
	Tdata	floor(double limit) const;
	Tdata	limit(double l) const;
	Tdata	apply(double (*func)(double)) const;

	// generic algorithm: aply a binary function to the data
	friend Tdata	apply2(double (*func)(double, double), const Tdata& a,
			const Tdata& b);
};

typedef	std::map<std::string, Tdata>		dmap_t;

} /* namespace meteo */

#endif /* _Tdata_h */
