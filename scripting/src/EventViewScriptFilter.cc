//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#include "pxl/scripting/EventViewScriptFilter.hh"

namespace pxl {

EventViewScriptFilter::EventViewScriptFilter()
{
}

EventViewScriptFilter::EventViewScriptFilter(EventView* object)
{
	if (object) {
		std::string script;
		
		if ( object->getUserRecord<std::string>("filter_script", script))
		{
			create(script.c_str(), "EventView filter script");
			_FilterFunction = getFunction("evaluate");
		}
	}
}

EventViewScriptFilter::~EventViewScriptFilter()
{
}

double EventViewScriptFilter::evaluate(EventView* object)
{
	std::vector<PyObject *> arguments;

	PyObject *argument = createObjectFromPointer("EventView", object, false);
	arguments.push_back(argument);	


	PyObject *doubleObject = call(_FilterFunction, arguments);
	double result = Script::objectToDouble(doubleObject);
	Script::releaseObject(doubleObject);

	return result;
}

} // namespace pxl

