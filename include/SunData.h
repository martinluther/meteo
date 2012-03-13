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

namespace meteo {

class SunData {
	int	sensorid;
	int	offset;
public:
	// construct the SunData object
	SunData(const std::string& station, int off);
	~SunData(void) { }

	std::vector<time_t>	operator()(int interval, time_t startkey,
		time_t endkey);
};

}
