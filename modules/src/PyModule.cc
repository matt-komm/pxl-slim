//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#include "pxl/modules/PyModule.hh"
#include "pxl/core/logging.hh"

namespace pxl
{
static Logger logger("pxl::PyModule");

PyModule::PyModule() :
		Module(), _Instance(0), _AnalyseMethod(0), _BeginRunMethod(0), _EndRunMethod(
				0), _BeginJobMethod(0), _EndJobMethod(0), _OnPublishMethod(0), _AnalyseMethodName(
				"analyse")
{

}

void PyModule::initialize()
{
	addOption("filename", "Filename of the script", std::string(),
			OptionDescription::USAGE_FILE_OPEN);
	addOption("script", "Script to execute", std::string(),
			OptionDescription::USAGE_TEXT_MULTILINE);
	addOption("parameter", "Parameters passed to the script", std::string(),
			OptionDescription::USAGE_TEXT_MULTILINE);
	addOption("class", "Class to use.", std::string());

	try
	{
		std::string script, classname;
		getOption("script", script);
		getOption("filename", _filename);
		getOption("class", classname);
		if (getAnalysis())
			_filename = getAnalysis()->findFile(_filename);
		if (script.empty() == false)
		{
			if (_Script.create(script, "PyModule Script") == false)
			{
				logger(LOG_LEVEL_ERROR,
						"PyModule: script could not be compiled.",
						_Script.fetchPythonError(_filename));
				throw std::runtime_error(
						"PyModule: script could not be compiled.");
			}
		}
		else if (_filename.empty() == false)
		{
			try
			{
				if (_Script.load(_filename) == false)
				{
					throw std::runtime_error(
							"PyModule: script " + _filename + " not found.");
				}
			} catch (std::exception &e)
			{
				std::string message = Script::fetchPythonError(_filename);
				logger(LOG_LEVEL_ERROR, "Script crashed during load:\n",
						message);
				addStatusMessage(message);
				throw std::runtime_error(
						std::string("PyModule::initialize: " + message)
								+ e.what());
			}
		}
		else
		{
			// just return if script or _filename is given
			return;
		}

		if (_Script.getContext() == 0)
		{
			std::string pyError = _Script.fetchPythonError(_filename);
			addStatusMessage(pyError);
			logger(LOG_LEVEL_ERROR, "PyModule: error in pythoncode", pyError);
			throw std::runtime_error(
					"PyModule: script could not be compiled. " + pyError);
		}


		// get class
		PyObject *cls = 0;
		if (!classname.empty())
		{
			cls = getScript().getFunction(classname.c_str());
			if (cls == 0) {
				throw std::runtime_error(
						std::string(
								"PyModule could not find class '"+classname+"'"));
			}
		}
		else
		{
			cls = getScript().getClassWithMethod(_AnalyseMethodName);
			if (cls == 0)
			{
				throw std::runtime_error(
						std::string(
								"PyModule with missing '" + _AnalyseMethodName
										+ "' method."));
			}
		}

		// create instance from class
		_Instance = Script::call(cls, 0);
		if (_Instance == 0)
		{
			throw std::runtime_error(
					std::string(
							"PyModule could not instanciate class."));
		}

		// create arguments, containing pointer tothis module
		std::vector<PyObject *> args;
		args.push_back(Script::createObjectFromPointer("Module", this, false));

		Script::call(getScript().getMethod(_Instance, "initialize"),
				Script::createArgumentList(args));

		_AnalyseMethod = getScript().getMethod(_Instance, _AnalyseMethodName);
		if (_AnalyseMethod == 0)
		{
			throw std::runtime_error(
					std::string(
							"PyModule with missing '" + _AnalyseMethodName
									+ "' method."));
		}

		loadDefaultMethods(_Instance);

	} catch (std::exception &e)
	{
		std::string message = Script::fetchPythonError(_filename);
		logger(LOG_LEVEL_ERROR, "Script crashed during initialize:", message);
		addStatusMessage(message);

		throw std::runtime_error(
				std::string("PyModule::initialize: " + message) + e.what());
	} catch (...)
	{
		throw std::runtime_error("PyModule::initialize: unknown exception");
	}

	Module::initialize();
}

PyModule::~PyModule()
{
	shutdown();
}

Script &PyModule::getScript()
{
	return _Script;
}

PyObject *PyModule::getInstance()
{
	return _Instance;
}

void PyModule::setAnalyseMethodName(const std::string &name)
{
	_AnalyseMethodName = name;
}

PyObject *PyModule::getAnalyseMethod()
{
	return _AnalyseMethod;
}

void PyModule::beginJob()
{
	beginJob("");
}

void PyModule::loadDefaultMethods(PyObject *object)
{
	_BeginRunMethod = Script::getMethod(object, "beginRun");
	_EndRunMethod = Script::getMethod(object, "endRun");
	_BeginJobMethod = Script::getMethod(object, "beginJob");
	_EndJobMethod = Script::getMethod(object, "endJob");
	_OnPublishMethod = Script::getMethod(object, "onPublish");
}

void PyModule::shutdown()
{
	Script::releaseObject(_BeginRunMethod);
	Script::releaseObject(_EndRunMethod);
	Script::releaseObject(_BeginJobMethod);
	Script::releaseObject(_EndJobMethod);
	Script::releaseObject(_OnPublishMethod);
	Script::releaseObject(_AnalyseMethod);
	Script::releaseObject(_Instance);
	_Script.destroy();
	Module::shutdown();
}

void PyModule::reload()
{
	logger(LOG_LEVEL_DEBUG, "Reloading Script");
	Script::releaseObject(_BeginRunMethod);
	Script::releaseObject(_EndRunMethod);
	Script::releaseObject(_BeginJobMethod);
	Script::releaseObject(_EndJobMethod);
	Script::releaseObject(_OnPublishMethod);
	Script::releaseObject(_AnalyseMethod);
	Script::releaseObject(_Instance);
	logger(LOG_LEVEL_DEBUG, "DEstroying old Script");
	_Script.destroy();

	// the module is reloaded, it don't has a status
	clearStatusMessages();
	logger(LOG_LEVEL_DEBUG, "Reloading module");
	Module::reload();
}

void PyModule::beginRun()
{
	try
	{
		if (_BeginRunMethod)
			Script::call(_BeginRunMethod, 0);
	} catch (std::exception &e)
	{
		std::string message = Script::fetchPythonError(_filename);
		logger(LOG_LEVEL_ERROR, "beginRun crashed in module", getName(),
				message);
		addStatusMessage(message);
		throw std::runtime_error(e.what());
	}
}

void PyModule::endRun()
{
	try
	{
		if (_EndRunMethod)
			Script::call(_EndRunMethod, 0);
	} catch (std::exception &e)
	{
		std::string message = Script::fetchPythonError(_filename);
		logger(LOG_LEVEL_ERROR, "endRun crashed in module", getName(), message);
		addStatusMessage(message);
		throw std::runtime_error(e.what());
	}
}

void PyModule::beginJob(const std::string &parameters)
{
	if (_BeginJobMethod == 0)
		return;
	try
	{
		std::vector<PyObject *> arguments;
		PyObject *parameterList = Script::parseParameters(parameters);
		arguments.push_back(parameterList);
		Script::call(_BeginJobMethod, arguments);
	} catch (std::exception& e)
	{
		std::string message = Script::fetchPythonError(_filename);
		logger(LOG_LEVEL_ERROR, "beginJob crashed in module", getName(),
				message);
		addStatusMessage(message);
		throw std::runtime_error(e.what());
	}
}

void PyModule::endJob()
{
	try
	{
		if (_EndJobMethod)
			Script::call(_EndJobMethod, 0);
	} catch (std::exception& e)
	{
		std::string message = Script::fetchPythonError(_filename);
		logger(LOG_LEVEL_ERROR, "endJob crashed in module", getName(), message);
		addStatusMessage(message);
		throw std::runtime_error(e.what());
	}

}

void PyModule::setOption(const std::string &name, const std::string &value)
{
	Module::setOption(name, value);
	if (name == "filename")
		reload();
}

} // namespace pxl
