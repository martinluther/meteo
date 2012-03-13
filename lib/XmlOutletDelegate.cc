/*
 * XmlOutletDelegate.cc
 * 
 * (c) 2008 Prof Dr Andreas Mueller
 * $Id: XmlOutletDelegate.cc,v 1.2 2009/01/10 19:00:25 afm Exp $ 
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <XmlOutletDelegate.h>
#include <ios>
#include <iostream>
#include <fstream>

namespace meteo {

XmlOutletDelegate::XmlOutletDelegate() {
}
XmlOutletDelegate::~XmlOutletDelegate() {
}
void	XmlOutletDelegate::write(const std::string& xml) {
	std::cout << xml << std::endl;
}

XmlFileOutletDelegate::XmlFileOutletDelegate(const std::string& _filename) : filename(_filename) {
}
XmlFileOutletDelegate::~XmlFileOutletDelegate() {
}
void	XmlFileOutletDelegate::write(const std::string& xml) {
	// open the file
	std::ofstream	outfile(filename.c_str(), std::ios_base::ate);

	// append the XML to the file
	outfile << xml;

	// close the file
	outfile.close();
}

XmlPipeOutletDelegate::XmlPipeOutletDelegate(const std::string& _command) : command(_command) {
}
XmlPipeOutletDelegate::~XmlPipeOutletDelegate() {
}

void	XmlPipeOutletDelegate::write(const std::string& xml) {
	FILE *p = popen(command.c_str(), "w");
	if (NULL == p) {
		return;
	}
	fwrite(xml.c_str(), xml.size(), 1, p);
	pclose(p);
}

} // namespace meteo
