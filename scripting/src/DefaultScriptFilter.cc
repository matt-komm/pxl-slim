//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#include "pxl/scripting/DefaultScriptFilter.hh"

#include <iostream>
#include <stdexcept>

namespace pxl {

DefaultScriptFilter::DefaultScriptFilter() : beginJobFunction(0), beginRunFunction(0), endRunFunction(0), endJobFunction(0)
{
}

DefaultScriptFilter::~DefaultScriptFilter()
{
	destroy();
}

bool DefaultScriptFilter::create(const std::string& script, const std::string& filename)
{
	if (Script::create(script, filename) == false)
		return false;

	beginJobFunction = getFunction("beginJob");
	beginRunFunction = getFunction("beginRun");
	endRunFunction = getFunction("endRun");
	endJobFunction = getFunction("endJob");

	return true;
}

void DefaultScriptFilter::destroy()
{
	releaseObject(beginJobFunction);
	releaseObject(beginRunFunction);
	releaseObject(endRunFunction);
	releaseObject(endJobFunction);

	Script::destroy();
}

void DefaultScriptFilter::beginJob(const std::string &parameters)
{
	if (beginJobFunction == 0)
		return;

	std::vector<PyObject *> arguments;
	PyObject *parameterList = parseParameters(parameters);
	arguments.push_back(parameterList);
   	call(beginJobFunction, arguments);
}

void DefaultScriptFilter::beginRun()
{
	if (beginRunFunction == 0)
		return;

	call(beginRunFunction, 0);
}


void DefaultScriptFilter::endRun()
{
	if (endRunFunction == 0)
		return;

	call(endRunFunction, 0);
}

void DefaultScriptFilter::endJob()
{
	if (endJobFunction == 0)
		return;

	call(endJobFunction, 0);
}

} // namespace pxl

