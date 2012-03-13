//
// SunData.h -- class to compute a map of booleans that indicate whether
//              the sun is up or not.
//
// (c) 2004 Dr. Andreas Mueller, Beratung und Entwicklung
//
#include <string>
#include <vector>
#include <sys/types.h>
#include <time.h>
#include <Sun.h>

namespace meteo {

class SunData {
	int	sensorid;
	int	offset;
	Sun	thesun;
	bool	compute;
public:
	// construct the SunData object
	SunData(const std::string& station, int off);
	~SunData(void) { }

	// accessor for the compute flag
	void	setCompute(bool _compute);
	bool	getCompute(void) const { return compute; }

	// compute the sunrise/sunset timekeys
	std::vector<time_t>	operator()(int interval, time_t startkey,
		time_t endkey);
private:
	std::vector<time_t>	from_database(int interval, time_t startkey,
		time_t endkey);
	std::vector<time_t>	computed(int interval, time_t startkey,
		time_t endkey);
};

}
