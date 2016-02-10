//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#include "pxl/modules/PyModuleModule.hh"
#include "pxl/modules/ModuleFactory.hh"

#include <cstdlib>

#include "pxl/core/Event.hh"
#include "pxl/core/BasicContainer.hh"
#include "pxl/hep/EventView.hh"
#include "pxl/core/logging.hh"

namespace pxl
{
static Logger logger("pxl::PyModuleModule");

void PyModuleModule::initialize()
{
	setAnalyseMethodName("process");
	PyModule::initialize();
}

const std::string& PyModuleModule::getStaticType()
{
	static const std::string name("PyModule");
	return name;
}

const std::string& PyModuleModule::getType() const
{
	return getStaticType();
}

bool PyModuleModule::isRunnable() const
{
	return false;
}

bool PyModuleModule::analyse(Sink *sink)
{
	if (getScript().getContext() == 0)
	{
		std::string message = "Python module " + getName() + ": No python code to execute. File <" + _filename +"> does not exist, cannot be read, or is empty." ; 
		logger(LOG_LEVEL_ERROR,message);
		throw std::runtime_error(message);
	}

	try
	{
		std::map<std::string, Sink>::iterator i = _sinks.begin();
		std::map<std::string, Sink>::iterator end = _sinks.end();
		while (i != end)
		{
			Serializable *obj = i->second.get();
			if (obj)
			{
				i->second.set(0);
				PyObject *pyobj = Script::createObjectFromPointer(
						"Serializable", obj, false);
				if (pyobj)
				{
					// create argument
					std::vector<PyObject *> args;
					args.push_back(pyobj);
					args.push_back(Script::createObjectFromString(i->first));

					PyObject *resultObject = getScript().call(
							getAnalyseMethod(), args);
					// check if result is string, if not raise exception 
					// Empty string to 'dump' the object 
					if (Script::objectIsString(resultObject))
					{
						std::string result = Script::objectToString(resultObject);
						if (!result.empty())
						{
							Source *source = getSource(result);
							source->setTargets(obj);
							return source->processTargets();
						}
					}
					else
					{
						throw std::runtime_error("PyModuleModule " + getName() + ": Non-String return value");
					}
				}
			}
			i++;
		}
	} catch (std::exception &e)
	{
		std::string message = Script::fetchPythonError();
		addStatusMessage(message);
		logger(LOG_LEVEL_ERROR, "Analysis crashed in module",getName(), message);
		throw std::runtime_error(getName() + ": " + e.what());
	} catch (...)
	{
		throw std::runtime_error(getName() + ": unknown exception");
	}

	return true;
}

} // namespace pxl

