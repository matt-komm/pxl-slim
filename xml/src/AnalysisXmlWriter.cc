//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2010 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#include "pxl/xml/AnalysisXmlWriter.hh"
#include "pxl/xml/UserRecordsXmlWriter.hh"
#include "pxl/modules.hh"
#include <cstring>
#include <string>
#include <iostream>

namespace pxl
{
static Logger logger("pxl::AnalysisXmlWriter");

void AnalysisXmlWriter::writeObject(Analysis *object, XmlStream &stream) const
{
	stream.startElementWithAttributes("analysis");
	stream.addAttribute("file_format_version", "0.4");
	stream.endAttributes(true);

	UserRecordsXmlWriter userRecordsXmlWriter;

	const std::vector<Module *> modules = object->getModules();
	for (std::vector<Module *>::const_iterator iter = modules.begin(); iter
			!= modules.end(); ++iter)
	{
		//open module element
		logger(LOG_LEVEL_DEBUG, "Writing Module");
		stream.startElementWithAttributes("module");
		logger(LOG_LEVEL_DEBUG, " * Name ", (*iter)->getName());
		stream.addAttribute("name", (*iter)->getName());

		if ((*iter)->isRunnable())
		{
			logger(LOG_LEVEL_DEBUG, " * RunIndex: ", (*iter)->getRunIndex());
			//stream.addAttribute("runIndex",(*iter)->getRunIndex());
		}
		logger(LOG_LEVEL_DEBUG, " * Type: ", (*iter)->getType());
		stream.addAttribute("type", (*iter)->getType());

		if ((*iter)->isEnabled())
			stream.addAttribute("isEnabled", "true");
		else
			stream.addAttribute("isEnabled", "false");

		stream.endAttributes(true);

		std::vector<OptionDescription> optionDescriptions =
				(*iter)->getOptionDescriptions();
		logger(LOG_LEVEL_DEBUG, " * ", optionDescriptions.size(), " Options:");
		for (size_t j = 0; j < optionDescriptions.size(); j++)
		{
			logger(LOG_LEVEL_DEBUG, "  - ", optionDescriptions[j].name);
			stream.startElementWithAttributes("option");
			stream.addAttribute("name", optionDescriptions[j].name);

			if (optionDescriptions[j].type == OptionDescription::TYPE_STRING)
			{
				stream.addAttribute("type", "string");
				stream.endAttributes(true);

				std::string val;
				(*iter)->getOption(optionDescriptions[j].name, val);
				logger(LOG_LEVEL_DEBUG, "    . Type String: ", val);
				stream.startCDATA();
				stream.writeValue(val);
				stream.endCDATA();
			}
			else if (optionDescriptions[j].type
					== OptionDescription::TYPE_STRING_VECTOR)
			{
				stream.addAttribute("type", "string_vector");
				//stream.addAttribute("TODO","NOT IMPLEMENTED YET");
				stream.endAttributes(true);
				std::vector<std::string> val;
				(*iter)->getOption(optionDescriptions[j].name, val);
				std::string out;
				out.append("(");
				for (size_t i = 0; i < val.size(); i++)
				{
					if (i != 0)
						out.append(",");
					out.append("\"");
					out.append(val[i]);
					out.append("\"");
				}
				out.append(")");

				stream.startCDATA();
				stream.writeValue(out);
				stream.endCDATA();
			}
			else if (optionDescriptions[j].type
					== OptionDescription::TYPE_DOUBLE)
			{
				stream.addAttribute("type", "double");
				stream.endAttributes(true);

				double val;
				(*iter)->getOption(optionDescriptions[j].name, val);
				logger(LOG_LEVEL_DEBUG, "    . Type Double: ", val);
				stream.writeValue(val);
			}
			else if (optionDescriptions[j].type == OptionDescription::TYPE_LONG)
			{
				stream.addAttribute("type", "long");
				stream.endAttributes(true);

				int64_t val;
				(*iter)->getOption(optionDescriptions[j].name, val);
				logger(LOG_LEVEL_DEBUG, "    . Type long: ", val);
				stream.writeValue (val);
			}
			else if (optionDescriptions[j].type
					== OptionDescription::TYPE_BOOLEAN)
			{
				stream.addAttribute("type", "boolean");
				stream.endAttributes(true);
				bool v;
				std::string val;
				(*iter)->getOption(optionDescriptions[j].name, v);

				if (v)
				{
					val = "True";
				}
				else
				{
					val = "False";
				}
				logger(LOG_LEVEL_DEBUG, "    . Type BOOL: ", val);
				stream.writeValue(val);
			}
			stream.endElement("option");
		}
		userRecordsXmlWriter.writeObject((*iter)->getUserRecords(), stream);
		stream.endElement("module");
	}

	// Write Connections
	const std::vector<Connection*> connections = object->getConnections();
	logger(LOG_LEVEL_DEBUG, "Writing ", connections.size(), " Connections");
	for (std::vector<Connection*>::const_iterator iter = connections.begin(); iter
			!= connections.end(); ++iter)
	{
		logger(LOG_LEVEL_DEBUG, "Connection", (*iter)->sourceModuleName,
				(*iter)->sourceName, " with ", (*iter)->sinkModuleName,
				(*iter)->sinkName);
		stream.startElement("connection");
		stream.startElementWithAttributes("source");
		stream.addAttribute("module", (*iter)->sourceModuleName);
		stream.addAttribute("name", (*iter)->sourceName);
		stream.endAttributes(true);
		stream.endElement("source");

		stream.startElementWithAttributes("sink");
		stream.addAttribute("module", (*iter)->sinkModuleName);
		stream.addAttribute("name", (*iter)->sinkName);
		stream.endAttributes(true);
		stream.endElement("sink");

		stream.endElement("connection");
	}
	userRecordsXmlWriter.writeObject(object->getUserRecords(), stream);

	stream.endElement("analysis");
}
} // namespace pxl
