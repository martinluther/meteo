/*
 * Graphics.h -- draw meteo data graphics specified in XML files
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: Graphics.h,v 1.10 2004/02/27 22:09:04 afm Exp $
 */
#ifndef _Graphics_h
#define _Graphics_h

#include <Frame.h>
#include <GraphWindow.h>
#include <Dataset.h>

namespace meteo {

class Graphics {
	Frame		*f;
	GraphWindow	*gw;

	// draw channels, used during construction
	void	drawChannels(const std::string& xpath, const Dataset *ds);
	// add drawing the sunrise/sunset info
	void	drawSun(const std::string& basepath);
public:
	// the constructor does everything
	Graphics(int interval, time_t end, bool aligncenter,
		const std::string& graphname, bool withdata);
	~Graphics(void);

	void	toFile(const std::string& filename);
	std::string	mapString(const std::string& url,
				const std::string& station) const;
	std::string	imageTagString(const std::string& filename) const;
};

} /* namespace meteo */

#endif /* _Graphics_h */
