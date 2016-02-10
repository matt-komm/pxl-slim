//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2010 Martin Erdmann        -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#define NOMINMAX

#include "pxl/modules/Analysis.hh"
#include "pxl/modules/Module.hh"
#include "pxl/modules/Modules.hh"
#include "pxl/modules/ModuleFactory.hh"
#include "pxl/core/logging.hh"
#include "pxl/core/functions.hh"
#include "pxl/core/Tokenizer.hh"
#include "pxl/core/Configuration.hh"

#include <algorithm>
#include <fstream>
#include <stack>
#include <limits>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#endif

namespace pxl
{

Logger logger("pxl::Analysis");

Analysis::Analysis()
{
	Modules::initialize();
}

Analysis::~Analysis()
{
	for (size_t i = 0; i < _modules.size(); ++i)
		if (_modules[i] != 0)
			_modules[i]->destroy();
	for (size_t i = 0; i < _connections.size(); ++i)
		if (_connections[i] != 0)
			delete _connections[i];
}

void Analysis::addDefaultSearchPaths()
{
	Configuration pxlConfig = Configuration::instance();
	Configuration::multimapType m = pxlConfig.getSection("paths");
	std::pair<Configuration::multimapIterator, Configuration::multimapIterator> values =
			m.equal_range("analysisSearchPaths");

	for (Configuration::multimapIterator iter = values.first;
			iter != values.second; ++iter)
	{
		addSearchPath((*iter).second);
	}
}

void Analysis::setOutputPath(const std::string &path)
{
	logger(LOG_LEVEL_INFO, "set output path:", path);
	_outputPath = path + PXL_PATH_SEPERATOR;
}

void Analysis::addSearchPath(const std::string &path)
{
	logger(LOG_LEVEL_INFO, "add search path:", path);
	_searchPaths.push_back(path + PXL_PATH_SEPERATOR);
}

std::string Analysis::findFile(const std::string &filename)
{
	if (filename.empty())
		return std::string();

	// checks whether filename is dcap path, no need for environmental expansion
	// TODO: implement generic method for any url
	if (filename.compare(0, 7, "dcap://") == 0)
		return std::string(filename);

	std::string expFilename = expandEnvironmentVariables(filename);
	std::ifstream file(expFilename.c_str());
	if (file.good())
		return expFilename;

	for (int i = 0; i < _searchPaths.size(); i++)
	{
		std::string filename2 = _searchPaths[i] + filename;
		logger(LOG_LEVEL_DEBUG, "findFile:", filename2);
		file.open(filename2.c_str());
		if (file.good())
			return filename2;
	}

	return filename;
}

const std::string &Analysis::getOutputPath() const
{
	return _outputPath;
}

std::string Analysis::getOutputFile(const std::string &file)
{
	if (file.empty())
		return std::string();

	std::string filename;
	bool absolute = isAbsolutePath(file);
	if (absolute)
		filename = file;
	else
		filename = _outputPath + file;

	logger(LOG_LEVEL_DEBUG, "getOutputFile final filename:", filename);

	std::stack<std::string> paths;
	std::string path = getParentDirectory(filename);
	while (path.empty() == false && isDirectory(path) == false)
	{
		logger(LOG_LEVEL_DEBUG, "getOutputFile parent directory:", path);
		paths.push(path);
		path = getParentDirectory(path);
	}

	while (paths.empty() == false)
	{
		logger(LOG_LEVEL_DEBUG, "getOutputFile create directory:", paths.top());
		createDirectory(paths.top());
		paths.pop();
	}

	return filename;
}

void Analysis::shutdown()
{
	for (vector_t::iterator i = _modules.begin(); i != _modules.end(); ++i)
	{
		(*i)->shutdown();
		(*i)->destroy();
	}
	_modules.clear();
}

void Analysis::setModuleOption(const std::string& moduleName,
		const std::string& optionName, const std::string& value)
{
	Module* module = getModule(moduleName);
	const OptionDescription &option = module->getOptionDescription(optionName);

	if (option.type == OptionDescription::TYPE_STRING)
	{
		logger(LOG_LEVEL_DEBUG, "Set string", moduleName, optionName, "to",
				value);
		module->setOption(optionName, value);
	}
	else if (option.type == OptionDescription::TYPE_LONG)
	{
		int64_t l = atol(trim(value).c_str());
		logger(LOG_LEVEL_DEBUG, "Set long", moduleName, optionName, "to", l);
		module->setOption(optionName, l);
	}
	else if (option.type == OptionDescription::TYPE_DOUBLE)
	{
		double f = atof(value.c_str());
		logger(LOG_LEVEL_DEBUG, "Set double", moduleName, optionName, "to", f);
		module->setOption(optionName, f);
	}
	else if (option.type == OptionDescription::TYPE_BOOLEAN)
	{
		bool a;
		std::string v = trim(value);
		if ((v == "true") || (v == "True"))
			a = true;
		else if ((v == "false") || (v == "False"))
			a = false;

		logger(LOG_LEVEL_DEBUG, "Set bool", moduleName, optionName, "to", a);
		module->setOption(optionName, a);
	}
	else if (option.type == OptionDescription::TYPE_STRING_VECTOR)
	{
		std::vector<std::string> stringVectorValue;
		Tokenizer tok;
		tok.setText(value);
		tok.setCharType('[', Tokenizer::WHITESPACE);
		tok.setCharType(']', Tokenizer::WHITESPACE);
		tok.setCharType('(', Tokenizer::WHITESPACE);
		tok.setCharType(')', Tokenizer::WHITESPACE);
		tok.setCharType('\"', Tokenizer::LITERAL);
		tok.setCharType('\'', Tokenizer::LITERAL);
		tok.setCharType(',', Tokenizer::DELIM);
		while (tok.hasNext())
		{
			std::string s = tok.next();
			stringVectorValue.push_back(s);
			logger(LOG_LEVEL_DEBUG, "Set string vector element", moduleName,
					optionName, "to - ", s);
		}
		module->setOption(optionName, stringVectorValue);
	}
	else
	{
		throw std::runtime_error(
				"Unrecognized option type? Something has gone terribly wrong here! ");
	}

}

const std::vector<Module *>& Analysis::getModules() const
{
	return _modules;
}

Module* Analysis::addModule(const char *moduleType, const char *moduleName,
		const char *runIndex)
{
	logger(LOG_LEVEL_DEBUG, "Adding Module Type:", safe_string(moduleType),
			" Name: ", safe_string(moduleName), " RunIndex: ",
			safe_string(runIndex));
	Module *module = ModuleFactory::instance().createModule(moduleType);
	if (module == NULL)
	{
		throw std::runtime_error(
				std::string("Analysis: module '") + moduleType
						+ "' not found!");
	}
	module->setAnalysis(this);
	module->setName(moduleName);
	_modules.push_back(module);

	module->initialize();

	if (runIndex != NULL)
	{
		module->setRunIndex(atol(runIndex));
	}
	return module;
}

void Analysis::addModule(Module *module)
{
	if (module == 0)
		return;
	if (hasModule(module->getName()))
	{
		throw std::runtime_error(
				std::string("Analysis: module '") + module->getName()
						+ "' not unique!");
	}
	module->setAnalysis(this);
	_modules.push_back(module);
}

void Analysis::removeModule(const char *moduleName)
{
	logger(LOG_LEVEL_DEBUG, "Removing connections to and from ", moduleName);
	std::vector<Connection*>::iterator citer = _connections.begin();
	while (citer != _connections.end())
	{
		if (((*citer)->sourceModuleName == moduleName)
				|| ((*citer)->sinkModuleName == moduleName))
		{
			logger(LOG_LEVEL_DEBUG, "  - Removing Connection ",
					(*citer)->sourceModuleName, (*citer)->sourceName, ", ",
					(*citer)->sinkModuleName, (*citer)->sinkName);
			Module* sourceModule = getModule((*citer)->sourceModuleName);
			sourceModule->getSource((*citer)->sourceName)->disconnect();
			delete (*citer);
			citer = _connections.erase(citer);
		}
		else
		{
			++citer;
		}
	}

	logger(LOG_LEVEL_DEBUG, "Removing Module ", moduleName);

	for (vector_t::iterator iter = _modules.begin(); iter != _modules.end();
			++iter)
	{
		Module *module = *iter;
		if (module->getName() == moduleName)
		{
			module->destroy();
			_modules.erase(iter);
			return;
		}
	}
	throw std::runtime_error("No such module!");
}

void Analysis::connectModules(const char *sourceModuleName,
		const char *sourceName, const char *sinkModuleName,
		const char *sinkName)
{
	Connection * connection = new Connection();
	connection->sourceModuleName = std::string(sourceModuleName);
	connection->sourceName = std::string(sourceName);
	connection->sinkModuleName = std::string(sinkModuleName);
	connection->sinkName = std::string(sinkName);

	logger(LOG_LEVEL_DEBUG, "Connecting Source:", connection->sourceModuleName,
			connection->sourceName, "with Sink:", connection->sinkModuleName,
			connection->sinkName);

	Module* sourceModule = getModule(sourceModuleName);
	Module* sinkModule = getModule(sinkModuleName);
	sourceModule->getSource(sourceName)->connect(sinkModule->getSink(sinkName));
	_connections.push_back(connection);
}

void Analysis::disconnectModules(const char *sourceModuleName,
		const char *sourceName, const char *sinkModuleName,
		const char *sinkName)
{

	logger(LOG_LEVEL_DEBUG, "Disconnecting", sourceModuleName, sourceName,
			sinkModuleName, sinkName);
	for (std::vector<Connection*>::iterator iter = _connections.begin();
			iter != _connections.end(); ++iter)
	{
		if (((*iter)->sourceModuleName == sourceModuleName)
				&& ((*iter)->sourceName == sourceName)
				&& ((*iter)->sinkModuleName == sinkModuleName)
				&& ((*iter)->sinkName == sinkName))
		{
			Module* sourceModule = getModule(sourceModuleName);
			sourceModule->getSource(sourceName)->disconnect();
			delete (*iter);
			_connections.erase(iter);
			return;
		}
	}
	throw std::runtime_error("No such connection!");
}

bool Analysis::hasModule(const std::string& name)
{
	for (vector_t::iterator it = _modules.begin(); it != _modules.end(); ++it)
	{
		if ((*it)->getName() == name)
		{
			return true;
		}
	}
	return false;
}

Module* Analysis::getModule(const std::string& name)
{
	for (vector_t::iterator it = _modules.begin(); it != _modules.end(); ++it)
	{
		if ((*it)->getName() == name)
		{
			return (*it);
		}
	}
	throw std::runtime_error(
			std::string("No Module with name") + name + " in Analysis!");
}

void Analysis::beginJob()
{
	logger(LOG_LEVEL_DEBUG, "Execute beginJob()");
	for (vector_t::iterator iModules = _modules.begin();
			iModules != _modules.end(); iModules++)
	{
		if (!(*iModules)->isEnabled())
		{
			// Only warn the user once at the beginning
			logger(LOG_LEVEL_WARNING, "Module", (*iModules)->getName(),
					"disabled!");
		}
		else
		{
			(*iModules)->beginJob();
		}
	}
}

void Analysis::beginRun()
{
	logger(LOG_LEVEL_DEBUG, "Execute beginRun()");
	for (vector_t::iterator iModules = _modules.begin();
			iModules != _modules.end(); iModules++)
	{
		if ((*iModules)->isEnabled())
			(*iModules)->beginRun();
	}
}

void Analysis::endRun()
{
	logger(LOG_LEVEL_DEBUG, "Execute endRun()");
	for (vector_t::iterator iModules = _modules.begin();
			iModules != _modules.end(); iModules++)
	{
		if ((*iModules)->isEnabled())
			(*iModules)->endRun();
	}
}

void Analysis::endJob()
{
	logger(LOG_LEVEL_DEBUG, "Execute endJob()");
	for (vector_t::iterator iModules = _modules.begin();
			iModules != _modules.end(); iModules++)
	{
		if ((*iModules)->isEnabled())
			(*iModules)->endJob();
	}
}

bool Analysis::nextModules(long &runIndex, std::vector<Module *> &modules)
{
	// find next higher or equal run index
	long newRunIndex = std::numeric_limits<long>::max();
	for (iterator_t i = _modules.begin(); i != _modules.end(); i++)
	{
		Module *m = *i;
		if (m->isRunnable() == false)
			continue;
		if (m->isEnabled() == false)
		{
			continue;
		}
		long ri = m->getRunIndex();
		if (ri >= runIndex && ri < newRunIndex)
			newRunIndex = ri;
	}

	if (newRunIndex == std::numeric_limits<long>::max())
		return false;

	runIndex = newRunIndex;

	// fill all modules with this run index into list
	for (iterator_t i = _modules.begin(); i != _modules.end(); i++)
	{
		Module *m = *i;
		if (m->isRunnable() == false)
			continue;
		if (m->isEnabled() == false)
			continue;
		if (m->getRunIndex() == runIndex)
		{
			modules.push_back(m);
		}
	}

	return true;
}

void Analysis::run()
{
	logger(LOG_LEVEL_INFO, "begin job");
	beginJob();

	long runIndex = 0;
	std::vector<Module *> modules;
	while (nextModules(runIndex, modules))
	{
		logger(LOG_LEVEL_INFO, "begin run", runIndex);
		beginRun();
		while (modules.size() != 0)
		{
			// process every startpoint
			std::vector<Module *>::iterator iCurrentModules = modules.begin();
			while (iCurrentModules != modules.end())
			{
				if ((*iCurrentModules)->analyse(0) == false)
				{
					iCurrentModules = modules.erase(iCurrentModules);
				}
				else
				{
					iCurrentModules++;
				}
			}
		}
		endRun();
		logger(LOG_LEVEL_INFO, "end run", runIndex);
		runIndex++;
	}

	endJob();
	logger(LOG_LEVEL_INFO, "end job");
}

const std::vector<Connection *>& Analysis::getConnections() const
{
	return _connections;
}

} // namespace pxl
