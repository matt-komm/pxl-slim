//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#include "pxl/modules/PySwitchModule.hh"
#include "pxl/modules/ModuleFactory.hh"

#include <cstdlib>

#include "pxl/core/Event.hh"
#include "pxl/core/BasicContainer.hh"
#include "pxl/core/functions.hh"
#include "pxl/hep/EventView.hh"
#include "pxl/core/logging.hh"

namespace pxl
{
static Logger logger("pxl::PySwitchModule");

void PySwitchModule::initialize()
{
	setAnalyseMethodName("switch");
	PyModule::initialize();
}
const std::string& PySwitchModule::getStaticType()
{
	static const std::string name("PySwitch");
	return name;
}

const std::string& PySwitchModule::getType() const
{
	return getStaticType();
}

bool PySwitchModule::isRunnable() const
{
	return false;
}

bool PySwitchModule::analyse(Sink *sink)
{
	if (getScript().getContext() == 0)
	{
		std::string message = "Python module " + getName() + ": No python code to execute. File <" + _filename +"> does not exist, cannot be read, or is empty." ; 
		logger(LOG_LEVEL_ERROR,message);
		throw std::runtime_error(message);
	}

	try
	{
		Serializable *obj = getSink("in")->get();
		if (obj)
		{
			PyObject *pyobj = Script::createObjectFromPointer("Serializable",
					obj, false);
			if (pyobj)
			{
				// create argument
				std::vector<PyObject *> args;
				args.push_back(pyobj);

				PyObject *resultObject = getScript().call(getAnalyseMethod(),
						args);
				std::string result = Script::objectToString(resultObject);
				if (!result.empty()) {
					Source *source = getSource(result);
					source->setTargets(obj);
					return source->processTargets();
				}
			}
		}
	}
	catch (std::exception &e)
	{
		std::string message = Script::fetchPythonError();
		addStatusMessage(message);
		logger(LOG_LEVEL_ERROR, "Analysis crashed in module",getName(), message);
		throw std::runtime_error(getName() + ": " + e.what());
	}
	catch (...)
	{
		throw std::runtime_error(getName() + ": unknown exception");
	}

	return true;
}

} // namespace pxl

