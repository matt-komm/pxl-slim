//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2010 Martin Erdmann        -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#include "pxl/xml/AnalysisXmlImport.hh"
#include "pxl/xml/XmlHandler.hh"
#include "pxl/xml/ModuleXmlHandler.hh"
#include "pxl/xml/UserRecordsXmlHandler.hh"
#include "pxl/core/PluginManager.hh"
#include "pxl/core/logging.hh"
#include "pxl/modules/Module.hh"

#include <cstring>
#include <stack>
#include <vector>

#include <expat.h>

#include <thread>
#include <chrono>

namespace pxl
{
static Logger logger("pxl::AnalysisXmlImport");

#define BUFFER_SIZE        8192

static void XMLCALL startHandler(void *data, const char *element,
		const char **attributes)
{
	AnalysisXmlImport *import = (AnalysisXmlImport *) data;
	import->startElement(element, attributes);
}

static void XMLCALL endHandler(void *data, const char *element)
{
	AnalysisXmlImport *import = (AnalysisXmlImport *) data;
	import->endElement(element);
}

static void XMLCALL startCdataSectionHandler(void *userData)
{
	AnalysisXmlImport *import = (AnalysisXmlImport *) userData;
	import->startCData();
}

static void XMLCALL endCdataSectionHandler(void *userData)
{
	AnalysisXmlImport *import = (AnalysisXmlImport *) userData;
	import->endCData();
}

static void XMLCALL dataHandler(void *data, const char *chars, int size)
{
	AnalysisXmlImport *import = (AnalysisXmlImport *) data;
	const char *trimmed = chars;
	int trimmed_size = size;

	// do not trim here: #389
#if 0
	// strip leading spaces
	while ((trimmed_size > 0) && (*trimmed <= 32))
	{
		trimmed_size--;
		trimmed++;
	}

	while ((trimmed_size > 0) && (*(trimmed + trimmed_size - 1) <= 32))
	{
		trimmed_size--;
	}
#endif

	import->dataElement(trimmed, trimmed_size);
}

AnalysisXmlImport::AnalysisXmlImport() :
	parser(0), file(0), depth(0), _handler(0)
{
	//_handler = new XmlHandler();
}

AnalysisXmlImport::~AnalysisXmlImport()
{
	if (_handler)
	{
		_handler->shutdown();
		delete _handler;
		_handler = 0;
	}
	close();
}

bool AnalysisXmlImport::open(const std::string &filename)
{
	logger(LOG_LEVEL_DEBUG, "open", filename);
	parser = XML_ParserCreate(NULL);
	if (parser == 0)
	{
		logger(LOG_LEVEL_ERROR, "Couldn't allocate memory for parser");
		return false;
	}

	file = fopen(filename.c_str(), "r");
	
	for (unsigned int i = 0; (i < 51) and (file == 0); ++i)
	{
	    std::this_thread::sleep_for(std::chrono::seconds(10+10*i));
	    logger(LOG_LEVEL_INFO,"Try again (",i,"/50) opening file ",filename);
	    file = fopen(filename.c_str(), "r");
	}
	if (file == 0)
	{
		logger(LOG_LEVEL_ERROR, "Couldn't open xml file", filename);
		return false;
	}
    logger(LOG_LEVEL_INFO,"Successfully opened file ",filename);
    
	XML_SetElementHandler(parser, startHandler, endHandler);
	XML_SetCharacterDataHandler(parser, dataHandler);
	XML_SetUserData(parser, this);
	XML_SetCdataSectionHandler(parser, startCdataSectionHandler,
			endCdataSectionHandler);
	std::string path = filename.substr(0, filename.find_last_of('/'));
	PluginManager::instance().loadPluginsFromDirectory(path);

	status = XML_STATUS_OK;

	return true;
}

std::vector<Module*> AnalysisXmlImport::parseInto(Analysis* analysis)
{
	logger(LOG_LEVEL_DEBUG, "Start parsing of xml file");
	if (!analysis)
	{
		throw std::runtime_error(
				"AnalysisXmlImport : Received Null pointer instead of Analysis Instance");
	}
	if (file == 0)
	{
		throw std::runtime_error(
				"AnalysisXmlImport : Cannot parse from unopened file");

	}

	_analysis = analysis;
	if (status == XML_STATUS_SUSPENDED)
	{
		status = XML_ResumeParser(parser);
		if (status == XML_STATUS_SUSPENDED)
			logger(LOG_LEVEL_DEBUG, "XML_STATUS_SUSPENDED");
	}

	bool running = true;
	while (running)
	{
		char buffer[BUFFER_SIZE];

		// read bytes from file
		size_t bytesRead = fread(buffer, 1, BUFFER_SIZE, file);
		if (ferror(file))
		{
			throw std::runtime_error("Error reading file");
		}

		if (bytesRead == 0)
			break;

		int final = (feof(file) != 0);
		status = XML_Parse(parser, buffer, bytesRead, final);
		if (status == XML_STATUS_ERROR)
		{
			char buffer[1024];
			sprintf(buffer, "Parse error at line %4.0lu:\n%.1000s\n",
					XML_GetCurrentLineNumber(parser), XML_ErrorString(
							XML_GetErrorCode(parser)));
			throw std::runtime_error(buffer);
		}
		else if (status == XML_STATUS_SUSPENDED)
		{
			running = false;
		}

		if (final)
			running = false;
	}
	return _defectiveModules;
}

void AnalysisXmlImport::startElement(const char *element,
		const char **attributes)
{
	// check whether this is depth 0
	if (depth == 0)
	{
		if (strcmp(element, "analysis") != 0)
		{
			logger(LOG_LEVEL_WARNING,
					"Top level element not of type analysis, found:", element);
		}
	}
	//on depth 0 we expect modules and connections
	else if (depth == 1)
	{
		if (strcmp(element, "module") == 0)
		{
			// Remove old handler before installing new
			if (_handler)
			{
				_handler->shutdown();
				delete _handler;
				_handler = 0;
			}
			_handler = new ModuleXmlHandler();
			_handler->initialize(attributes);
		}
		else if (strcmp(element, "connection") == 0)
		{
			// Remove old handler before installing new
			if (_handler)
			{
				_handler->shutdown();
				delete _handler;
				_handler = 0;
			}
			_handler = new ConnectionXmlHandler();
			_handler->initialize(attributes);
		}
		else if (strcmp(element, "userrecords") == 0)
		{
			// Remove old handler before installing new
			if (_handler)
			{
				_handler->shutdown();
				delete _handler;
				_handler = 0;
			}
			_handler = new UserRecordsXmlHandler(&_analysis->getUserRecords());
		}

	}
	else
	{
		_handler->startElement(element, attributes);
	}
	depth += 1;
}

void AnalysisXmlImport::endElement(const char *element)
{
	depth -= 1;

	if (depth == 0)
	{
		// we are back at level 0 and shutdown the handler, as the analysis
		// is finshed
		_handler->shutdown();
		delete _handler;
		_handler = 0;
	}
	else if (depth == 1)
	{
		// we have read in a new module/connection and will process it
		ModuleXmlHandler *moduleXmlHandler =
				dynamic_cast<ModuleXmlHandler*> (_handler);
		if (moduleXmlHandler)
		{
			// A module has been completly read in. Add the module and set
			// the options
			Module* module;
			try
			{
				module = _analysis->addModule(
						moduleXmlHandler->getModuleType().c_str(),
						moduleXmlHandler->getModuleName().c_str(),
						moduleXmlHandler->getRunIndex().c_str());

				if ((moduleXmlHandler->getEnabledStatus() == "no")
						|| (moduleXmlHandler->getEnabledStatus() == "false")
						|| (moduleXmlHandler->getEnabledStatus() == "0"))
				{
					module->setEnabled(false);
				}
				else
				{
					module->setEnabled(true);
				}

				for (std::vector<Option>::const_iterator iter =
						moduleXmlHandler->getModuleOptions().begin(); iter
						!= moduleXmlHandler->getModuleOptions().end(); ++iter)
				{
					logger(LOG_LEVEL_DEBUG, "Set Option '", iter->name, "' (",
							iter->type, ") -> '", iter->value, "'");
					_analysis->setModuleOption(
							moduleXmlHandler->getModuleName(), iter->name,
							iter->value);
				}
			} catch (std::exception &e)
			{
				logger(
						LOG_LEVEL_ERROR,
						"Error Loading Module",
						moduleXmlHandler->getModuleName().c_str(), "- no options for this module have been set!");
				module
						= _analysis->getModule(
								moduleXmlHandler->getModuleName());
				//module->setEnabled(false);
				module->addStatusMessage(std::string(
						"Exception during xml import: ") + e.what());
				_defectiveModules.push_back(module);
			}

			// set user Records to the module
			module->setUserRecords(moduleXmlHandler->getUserRecord());

		}
		else
		{
			// Process Connection: This will fail if modules are not created
			// first!
			ConnectionXmlHandler *connectionXmlHandler =
					dynamic_cast<ConnectionXmlHandler*> (_handler);
			if (connectionXmlHandler)
			{
				_analysis->connectModules(
						connectionXmlHandler->getSourceModuleName().c_str(),
						connectionXmlHandler->getSourceName().c_str(),
						connectionXmlHandler->getSinkModuleName().c_str(),
						connectionXmlHandler->getSinkName().c_str());
			}
		}
		_handler->endElement(element);
	}
	else
	{
		_handler->endElement(element);
	}
}

void AnalysisXmlImport::startCData()
{
	if (_handler)
		_handler->startCData();
}

void AnalysisXmlImport::endCData()
{
	if (_handler)
		_handler->endCData();
}

// data Elements are processed by the installed handler
void AnalysisXmlImport::dataElement(const char *chars, int size)
{
	if (_handler)
		_handler->dataElement(chars, size);
}

// Clean up the Import class
void AnalysisXmlImport::close()
{
	if (parser)
	{
		XML_ParserFree(parser);
		parser = 0;
	}

	if (file)
	{
		fclose(file);
		file = 0;
	}
}

} // namespace pxl
