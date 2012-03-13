/*
 * XmlOutletDelegate.h -- classes to delegate writing of the XML
 *
 * (c) 2008 Prof Dr Andreas Mueller
 * $Id: XmlOutletDelegate.h,v 1.2 2009/01/10 19:00:23 afm Exp $
 */
#ifndef _XmlOutletDelegate_h
#define _XmlOutletDelegate_h

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <string>

namespace meteo {

class XmlOutletDelegate {
public:
	XmlOutletDelegate();
	virtual ~XmlOutletDelegate();
	virtual void	write(const std::string& xml);
};

class XmlFileOutletDelegate : public XmlOutletDelegate {
	const std::string	filename;
public:
	XmlFileOutletDelegate(const std::string& filename);
	virtual	~XmlFileOutletDelegate();
	virtual void	write(const std::string& xml);
};

class XmlPipeOutletDelegate : public XmlOutletDelegate {
	const std::string	command;
public:
	XmlPipeOutletDelegate(const std::string& command);
	virtual	~XmlPipeOutletDelegate();
	virtual void	write(const std::string& xml);
};


} // namespace meteo

#endif /* XmlOutletDelegate_h */
