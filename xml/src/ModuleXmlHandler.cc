//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2010 Martin Erdmann        -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------
#include "pxl/core/logging.hh"
#include "pxl/core/functions.hh"
#include "pxl/xml/ModuleXmlHandler.hh"
#include "pxl/xml/UserRecordsXmlHandler.hh"
#include <cstring>

namespace pxl
{
static Logger logger("pxl::ModuleXmlHandler");

ModuleXmlHandler::~ModuleXmlHandler()
{
	safe_delete(_childHandler);
}

void ModuleOptionHandler::initialize(const char **attributes)
{
	logger(LOG_LEVEL_DEBUG, " Option Handler: Initialize option:");
	option.name = findAttribute("name", attributes);
	option.type = findAttribute("type", attributes);
	logger(LOG_LEVEL_DEBUG, "    * name: ", option.name);
	logger(LOG_LEVEL_DEBUG, "    * type: ", option.type);
	XmlHandler::initialize(attributes);
}

void ModuleOptionHandler::doEndElement(const char *chars)
{
	option.value = data;
	logger(LOG_LEVEL_DEBUG, "  Option '", option.name, "' (", option.type,
			") -> '", option.value, "'");
}

const Option ModuleOptionHandler::getOption() const
{
	return option;
}

void ModuleXmlHandler::initialize(const char **attributes)
{
	// for xml files created with vispa up to 0.4, there was an additional
	// id which was used as identifier for modules and conncetions. To
	// keep this xml files working, I check whether there is an id
	// attribute and use it instead of the name.

	if (findAttribute("id", attributes))
	{
		logger(
				LOG_LEVEL_WARNING,
				"Id found in module attributes! Will  be used as module name, instead of the attribute name. This is deprecated and will be removed soon");
		name.append(findAttribute("id", attributes));
	}
	else
	{
		name.append(findAttribute("name", attributes));
	}
	type.append(findAttribute("type", attributes));
	logger(LOG_LEVEL_DEBUG, "Initialize Module '", name, "' (", type, ")");
	if (findAttribute("runIndex", attributes) != 0)
	{
		runIndex.append(runIndex.append(findAttribute("runIndex", attributes)));
		logger(LOG_LEVEL_DEBUG, "  Run Index: '", findAttribute("runIndex",
				attributes), "'");
	}
	if (findAttribute("isEnabled", attributes) != 0)
	{
		isEnabled.append(findAttribute("isEnabled", attributes));
		logger(LOG_LEVEL_DEBUG, "  Enabled status:'", findAttribute(
				"isEnabled", attributes), "'");
	}
	XmlHandler::initialize(attributes);
}

void ModuleXmlHandler::doStartElement(const char *element,
		const char **attributes)
{
	if (strcmp(element, "option") == 0)
	{
		logger(LOG_LEVEL_DEBUG,
				" Installing new Option Handler as Childhandler");
		safe_delete(_childHandler);
		_childHandler = new ModuleOptionHandler();
		setChildHandler(_childHandler);
		_childHandler->initialize(attributes);
	}
	else if (strcmp(element, "userrecords") == 0)
	{
		logger(LOG_LEVEL_DEBUG,
				" Installing new User Record Handler as Childhandler");
		safe_delete (_childHandler);
		_childHandler = new UserRecordsXmlHandler(&_userRecord);
		setChildHandler(_childHandler);
	}
}

void ModuleXmlHandler::doEndChildHandler(XmlHandler *childHandler)
{
	ModuleOptionHandler* _moduleOptionHandler =
			dynamic_cast<ModuleOptionHandler*> (childHandler);
	if (_moduleOptionHandler)
	{
		options.push_back(_moduleOptionHandler->getOption());
	}
	UserRecordsXmlHandler* _userRecordHandler =
			dynamic_cast<UserRecordsXmlHandler*> (childHandler);
	if (_userRecordHandler)
	{
		logger(LOG_LEVEL_DEBUG, " got User Record" + _userRecord.toString());
	}
}

const std::string& ModuleXmlHandler::getModuleName() const
{
	return name;
}

const std::string& ModuleXmlHandler::getModuleType() const
{
	return type;
}

const std::string& ModuleXmlHandler::getRunIndex() const
{
	return runIndex;
}

const std::string& ModuleXmlHandler::getEnabledStatus() const
{
	return isEnabled;
}

const std::vector<Option>& ModuleXmlHandler::getModuleOptions() const
{
	return options;
}

void ConnectionXmlHandler::doStartElement(const char *element,
		const char **attributes)
{
	if (strcmp(element, "source") == 0)
	{
		sourceModuleName = findAttribute("module", attributes);
		sourceName = findAttribute("name", attributes);
	}
	else if (strcmp(element, "sink") == 0)
	{
		sinkModuleName = findAttribute("module", attributes);
		sinkName = findAttribute("name", attributes);
	}
}

const std::string& ConnectionXmlHandler::getSourceModuleName() const
{
	return sourceModuleName;
}

const std::string& ConnectionXmlHandler::getSourceName() const
{
	return sourceName;
}

const std::string& ConnectionXmlHandler::getSinkModuleName() const
{
	return sinkModuleName;
}

const std::string& ConnectionXmlHandler::getSinkName() const
{
	return sinkName;
}

} // namespace pxl
