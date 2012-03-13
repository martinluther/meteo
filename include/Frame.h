/*
 * Frame.h -- a frame is something to draw into, it's the stuff around the
 *            the meteo data graph
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _Frame_h 
#define _Frame_h

#include <Dimension.h>
#include <Rectangle.h>
#include <Color.h>
#include <Label.h>

namespace meteo {

class	frame_internals;

typedef enum linestyle_e {
	solid = 0, dotted = 1, dashed = 2
} linestyle;

class Frame {
	Dimension	outer;
	frame_internals	*fi;
	Color		foreground, background;
	// construct the internal GD stuff
	void	setupInternals(void);
public:
	// construction an destruction
	Frame(const Dimension& o);
	Frame(const Frame& f);
	~Frame(void);
	Frame&	operator=(const Frame& other);

	// foreground and background colors
	const Color&	getForeground(void) const { return foreground; }
	const Color&	getBackground(void) const { return background; }
	void	setForeground(const Color& c);
	void	setForeground(const std::string& c);
	void	setBackground(const Color& c);
	void	setBackground(const std::string& c);
	Color	getColorFromHexString(const std::string& c) const;

	// drawing functions (this functions perform some limited form
	// of clipping)
	void	drawRectangle(const Rectangle& rectangle, const Color& color);
	void	drawLine(const Point& p1, const Point& p2, const Color& color,
			linestyle style);
	void	drawText(const std::string& text, const Point& start,
			const Color& color, bool horizontal = true);
	void	drawLetter(const char c, const Point& center,
			const Color& color);

	// specialised drawing functions
	void	drawLabel(const std::string& text, bool left, labelalign_t align);
	void	drawLabel(const Label& label, bool left);

	// output of image to a file
	void	toFile(const std::string& filename);
};

} /* namespace meteo */

#endif /* _Frame_h */
