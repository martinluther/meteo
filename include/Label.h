/*
 * Label.h -- class to represent the text labels at the right and left of a
 *            graph
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: Label.h,v 1.2 2004/02/25 23:52:34 afm Exp $
 */
#ifndef _Label_h 
#define _Label_h

#include <string>

namespace meteo {

// the labelalign_t represents the alignment of a label
typedef enum labelalign_e {
	top, center, bottom
} 	labelalign_t;

class Label {
	std::string	text;
	labelalign_t	align;
public:
	Label(const std::string& t, labelalign_t a) : text(t), align(a) { }
	Label(const std::string& xpath);
	~Label(void) { }
	const std::string&	getText(void) const { return text; }
	const labelalign_t	getAlign(void) const { return align; }
};

} /* namespace meteo */

#endif /* _Label_h */
