/*
 * Calibrator.h -- perform a linear transformation on data
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _Calibrator_h
#define _Calibrator_h

namespace meteo {

class Calibrator {
	double	slope, y0;
	bool	topclip, bottomclip;
	double	topcliplevel, bottomcliplevel;
public:
	Calibrator(void);
	Calibrator(double m, double y);
	Calibrator(double x1, double y1, double x2, double y2);
	~Calibrator(void);
	void setTopclip(double t) { topcliplevel = t; topclip = true; }
	void setBottomclip(double t) { bottomcliplevel = t; bottomclip = true; }
	double	operator()(const double x) const;
	double	getSlope(void) const { return slope; }
	double	getY0(void) const { return y0; }
};

} /* namespace meteo */

#endif /* _Calibrator_h */
