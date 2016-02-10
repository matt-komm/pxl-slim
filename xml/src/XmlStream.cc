//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#include "pxl/xml/XmlStream.hh"
#include "pxl/core/logging.hh"

namespace pxl
{
static Logger logger("pxl::XmlStream");

void XmlStream::pad()
	{
		if (depth <= 0)
			return;
		for (size_t i = 0; i < depth; i++)
			out << "  ";
	}

void XmlStream::startElementWithAttributes(const std::string &name)
	{
		pad();
		out << "<" << name;
		depth++;
	}

	void XmlStream::startElement(const std::string &name)
	{
		pad();
		out << "<" << name << ">" << std::endl;
		depth++;
	}

	void XmlStream::addAttribute(const std::string &key, const std::string &value)
	{
		out << " " << key << "=\"" << value << "\" ";
	}

	void XmlStream::endAttributes(bool newline)
	{
		out << ">";
		if (newline)
			out << std::endl;
	}

	void XmlStream::endElement(const std::string &name, bool padding)
	{
		depth--;
		if (padding)
			pad();
		out << "</" << name << ">" << std::endl;
	}

	std::ostream& XmlStream::getStream()
	{
		return out;
	}

	void XmlStream::writeTextElement(const std::string &name, const std::string &text)
	{
		pad();
		out << "<" << name << ">";
		startCDATA();
		out << text;
		endCDATA();
		out << "</" << name << ">" << std::endl;
	}

	void XmlStream::startCDATA()
	{
		out << "<![CDATA[";
	}

	void XmlStream::endCDATA()
	{
		out << "]]>";
	}


} // namespace pxl
