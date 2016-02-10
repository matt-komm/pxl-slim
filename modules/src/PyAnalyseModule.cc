//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#include "pxl/modules/PyAnalyseModule.hh"
#include "pxl/modules/ModuleFactory.hh"

#include <cstdlib>

#include "pxl/core/Event.hh"
#include "pxl/core/BasicContainer.hh"
#include "pxl/hep/EventView.hh"
#include "pxl/core/logging.hh"

namespace pxl
{
static Logger logger("pxl::PyAnalyseModule");


void PyAnalyseModule::initialize() 
{
	addSink("in", "events to be analysed");
	addSource("out", "analysed events");
	setAnalyseMethodName("analyse");
	PyModule::initialize();
}

const std::string& PyAnalyseModule::getStaticType()
{
	static const std::string name("PyAnalyse");
	return name;
}

const std::string& PyAnalyseModule::getType() const
{
	return getStaticType();
}

bool PyAnalyseModule::isRunnable() const
{
	return false;
}

bool PyAnalyseModule::analyse(Sink *sink)
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
		if (obj == 0)
			return true;

		PyObject *pyobj = Script::createObjectFromPointer("Serializable", obj,
				false);
		if (pyobj == 0)
			return false;

		// create argument
		std::vector<PyObject *> args;
		args.push_back(pyobj);

		// call 'analyse'
		getScript().call(getAnalyseMethod(), args);

		getSource("out")->setTargets(obj);
		return getSource("out")->processTargets();
	} catch (std::exception &e)
	{
		std::string message = Script::fetchPythonError();
		addStatusMessage(message);
		logger(LOG_LEVEL_ERROR, "Analysis crashed in module",getName(), message);
		throw std::runtime_error("PyAnalyseModule " + getName() + ": " + e.what());
	} catch (...)
	{
		throw std::runtime_error("PyAnalyseModule " +getName() + ": unknown exception");
	}

	return true;
}

} // namespace pxl

