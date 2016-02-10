//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#ifndef PXL_XML_XML_HANDLER
#define PXL_XML_XML_HANDLER

#include "pxl/core/macros.hh"

#include <string>

namespace pxl
{

/// Providing basic routines for XML import and export
class PXL_DLL_EXPORT XmlHandler
{
private:
	XmlHandler *childHandler;
	int childHandlerLevel;
	int skippingLevel;

protected:
	std::string data;
	bool haveCdata, parseCdata;
	const char *findAttribute(const char *name, const char **attributes);

public:

	XmlHandler();
	virtual ~XmlHandler();
	void setChildHandler(XmlHandler *handler);

	XmlHandler * getChildHandler();

	void skipCurrentElement();
	void startElement(const char *element, const char **attributes);
	void endElement(const char *element);
	void startCData();
	void endCData();
	void dataElement(const char *data, int size);

	virtual void initialize(const char **attributes);
	virtual void shutdown();

	virtual void doStartElement(const char *element, const char **attributes);
	virtual void doEndElement(const char *element);
	virtual void doEndChildHandler(XmlHandler *childHandler);

};

}

#endif
