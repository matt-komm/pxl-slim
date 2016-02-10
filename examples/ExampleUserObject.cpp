
//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//          http://pxl.sourceforge.net      -
// Copyright (C) 2009-2010 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Contact: pxl-users@lists.sourceforge.net -
//-------------------------------------------

#include <iostream>
#include "ExampleUserObject.h"
#include "pxl/core/ObjectFactory.hh"

void ExampleUserObject::testMethod()
{
	std::cout << "TestMethod\n";
}

static bool _initialized = false;

static pxl::ObjectProducerTemplate<ExampleUserObject> _ExampleUserObject_Producer;

void initialize()
{
	if (_initialized)
		return;
	_ExampleUserObject_Producer.initialize();
}

void shutdown()
{
	if (!_initialized)
		return;
	_ExampleUserObject_Producer.shutdown();
}


PXL_PLUGIN_INIT


