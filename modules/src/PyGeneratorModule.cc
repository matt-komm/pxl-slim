//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#include "pxl/modules/PyGeneratorModule.hh"
#include "pxl/modules/ModuleFactory.hh"
#include "pxl/core/logging.hh"
#include "pxl/core/Object.hh"
#include "pxl/core/BasicContainer.hh"
#include "pxl/hep/EventView.hh"

#include <cstdlib>

namespace pxl
{
static Logger logger("pxl::PyGeneratorModule");

PyGeneratorModule::PyGeneratorModule() :
	PyModule(), _EndIndex(-1), _ObjectCount(0), _GenerateFunction(0),
			_Object(0)
{

}

void PyGeneratorModule::initialize() 
{
	addSource("output", "generated events");
	addOption("number", "number of objects to generate.", int64_t(0));
	setAnalyseMethodName("generate");
	PyModule::initialize();
}

const std::string& PyGeneratorModule::getStaticType()
{
	static const std::string name("PyGenerator");
	return name;
}

const std::string& PyGeneratorModule::getType() const
{
	return getStaticType();
}

bool PyGeneratorModule::isRunnable() const
{
	return true;
}

void PyGeneratorModule::beginJob() 
{
	getOption("number", _EndIndex);
	PyModule::beginJob();
}

void PyGeneratorModule::endJob() 
{
	safe_delete(_Object);
	PyModule::endJob();
}

bool PyGeneratorModule::analyse(Sink *sink) 
{
	if (getScript().getContext() == 0)
	{
		std::string message = "Python module " + getName() + ": No python code to execute. File <" + _filename +"> does not exist, cannot be read, or is empty." ; 
		logger(LOG_LEVEL_ERROR,message);
		throw std::runtime_error(message);
	}

	try
	{
		safe_delete(_Object);

		if (_EndIndex > 0 && _ObjectCount >= _EndIndex)
		{
			logger(LOG_LEVEL_INFO, getName(), " :  generated", _ObjectCount,
					" Objects");
			return false;
		}

		PyObject *pyobj = Script::call(getAnalyseMethod(), 0);
		_Object = Script::objectToSerializable(pyobj, true);
		Script::releaseObject(pyobj);

		if (_Object)
		{
			_ObjectCount++;
			getSource("output")->setTargets(_Object);
			return getSource("output")->processTargets();
		}
		else
		{
			logger(LOG_LEVEL_INFO, getName(), " :  generated", _ObjectCount,
					" Objects");
			return false;
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

