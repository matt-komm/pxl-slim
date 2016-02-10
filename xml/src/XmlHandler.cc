//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#include "pxl/xml/XmlHandler.hh"

#include <cstring>
#include <iostream>

namespace pxl
{

XmlHandler::XmlHandler() :
	childHandler(0), skippingLevel(0)
{

}

XmlHandler::~XmlHandler()
{
}

void XmlHandler::setChildHandler(XmlHandler *newChildHandler)
{
	childHandler = newChildHandler;
	childHandlerLevel = 1;
}

XmlHandler * XmlHandler::getChildHandler()
{
	return childHandler;
}

void XmlHandler::skipCurrentElement()
{
	skippingLevel = 1;
}

void XmlHandler::initialize(const char **attributes)
{
	parseCdata = false;
	haveCdata = false;
}

void XmlHandler::shutdown()
{
}

void XmlHandler::startElement(const char *element, const char **attributes)
{
	if (childHandler && childHandlerLevel != 0)
	{
		childHandler->startElement(element, attributes);
		childHandlerLevel++;
	}
	else if (skippingLevel)
		skippingLevel += 1;
	else
		doStartElement(element, attributes);
}

void XmlHandler::endElement(const char *element)
{
	if (childHandler && childHandlerLevel != 0)
	{
		childHandler->endElement(element);
		childHandlerLevel--;
		if (childHandlerLevel == 0)
		{
			childHandler->shutdown();
			doEndChildHandler(childHandler);
		}
	}
	else if (skippingLevel)
		skippingLevel -= 1;
	else
		doEndElement(element);

	parseCdata = false;
	haveCdata = false;
}

void XmlHandler::startCData()
{
	if (childHandler && childHandlerLevel != 0)
		childHandler->startCData();
	else
	{
		data.clear();
		parseCdata = true;
		haveCdata = false;
	}
}

void XmlHandler::endCData()
{
	if (childHandler && childHandlerLevel != 0)
		childHandler->endCData();
	else
	{
		parseCdata = false;
		haveCdata = true;
	}
}

// data Elements are processed by the installed handler
void XmlHandler::dataElement(const char *chars, int size)
{
	if (childHandler && childHandlerLevel != 0)
		childHandler->dataElement(chars, size);
	else if (skippingLevel == 0 && haveCdata == false)
	{
		data.append(chars, size);
	}
}

void XmlHandler::doStartElement(const char *element, const char **attributes)
{
}

void XmlHandler::doEndElement(const char *element)
{
}

void XmlHandler::doEndChildHandler(XmlHandler *childHandler)
{
}

const char *XmlHandler::findAttribute(const char *name, const char **attributes)
{
	for (size_t i = 0; attributes[i]; i += 2)
	{
		if (strcmp(attributes[i], name) == 0)
			return attributes[i + 1];
	}
	return 0;
}

} // namespace pxl
