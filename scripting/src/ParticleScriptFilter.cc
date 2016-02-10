//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#include "pxl/scripting/ParticleScriptFilter.hh"

namespace pxl
{

ParticleScriptFilter::ParticleScriptFilter() :
	DefaultScriptFilter()
{
}

ParticleScriptFilter::ParticleScriptFilter(Particle *object)
{
	if (object)
	{
		std::string script;

		if (object->getUserRecord<std::string> ("filter_script", script))
		{
			create(script.c_str(), "Particle filter script");
			_FilterFunction = getFunction("evaluate");
		}
	}
}

ParticleScriptFilter::~ParticleScriptFilter()
{
}

double ParticleScriptFilter::evaluate(Particle* object)
{
	std::vector<PyObject *> arguments;

	PyObject *argument = createObjectFromPointer("Particle", object, false);
	arguments.push_back(argument);

	PyObject *doubleObject = call(_FilterFunction, arguments);
	double result = Script::objectToDouble(doubleObject);
	Script::releaseObject(doubleObject);

	return result;
}

} // namespace pxl

