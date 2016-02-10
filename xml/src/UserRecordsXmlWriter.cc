//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009 Martin Erdmann        -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------
#include "pxl/xml/UserRecordsXmlWriter.hh"

using namespace pxl;

void UserRecordsXmlWriter::writeObject(const UserRecords& object, XmlStream &stream) const
{
	stream.startElement("userrecords");
	std::map<std::string, Variant>::const_iterator it;
	for (it = object.begin(); it!=object.end(); ++it)
	{
		stream.startElementWithAttributes("entry");
		stream.addAttribute("name",it->first);
		stream.addAttribute("type",it->second.getTypeName());
		stream.endAttributes(true);
		std::stringstream s;
		if (it->second.getType() == Variant::TYPE_STRING)
		{
			s << "<![CDATA[";
		}

		s << it->second;

		if (it->second.getType() == Variant::TYPE_STRING)
		{
			s << "]]>";
		}
		stream.writeValue(s.str());
		stream.endElement("entry");
	}
	stream.endElement("userrecords");
}


