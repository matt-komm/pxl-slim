//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009 Martin Erdmann        -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#ifndef XMLSTREAM_HH_
#define XMLSTREAM_HH_

#include <iostream>
#include "pxl/core/macros.hh"

namespace pxl
{
class PXL_DLL_EXPORT XmlStream
{
	std::ostream &out;
	int depth;
	void pad();

public:
	XmlStream(std::ostream &stream) :
		out(stream), depth(0)
	{
	}

	void startElementWithAttributes(const std::string &name);

	void startElement(const std::string &name);

	void addAttribute(const std::string &key, const std::string &value);

	void endAttributes(bool newline = true);

	void endElement(const std::string &name, bool padding = true);

	std::ostream &getStream();

	void writeTextElement(const std::string &name, const std::string &text);

	template<typename T> void writeField(const std::string &name,
			const T &value)
	{
		pad();
		out << "<" << name << ">" << value << "</" << name << ">" << std::endl;
	}

	template<typename T> void writeValue(const T &value)
	{
		//		pad();
		out << value;
		//<< std::endl;
	}

	void startCDATA();

	void endCDATA();
};

} // namespace pxl

#endif /* XMLSTREAM_HH_ */
