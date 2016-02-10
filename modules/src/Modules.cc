//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#include "pxl/modules.hh"

namespace pxl
{

static bool _initialized = false;

static ModuleProducerTemplate<InputModule> _InputModuleProducer;
static ModuleProducerTemplate<OutputModule> _OutputModuleProducer;
static ModuleProducerTemplate<PyAnalyseModule> _PyAnalyseModuleProducer;
static ModuleProducerTemplate<PyDecideModule> _PyDecideModuleProducer;
static ModuleProducerTemplate<PyGeneratorModule> _PyGeneratorModuleProducer;
static ModuleProducerTemplate<PyModuleModule> _PyModuleModuleProducer;
static ModuleProducerTemplate<PySwitchModule> _PySwitchModuleProducer;

void Modules::initialize()
{
	if (_initialized)
		return;

	_InputModuleProducer.initialize();
	_OutputModuleProducer.initialize();
	_PyAnalyseModuleProducer.initialize();
	_PyDecideModuleProducer.initialize();
	_PyGeneratorModuleProducer.initialize();
	_PyModuleModuleProducer.initialize();
	_PySwitchModuleProducer.initialize();

	_initialized = true;
}

void Modules::shutdown()
{
	if (_initialized == false)
		return;

	_InputModuleProducer.shutdown();
	_OutputModuleProducer.shutdown();
	_PyAnalyseModuleProducer.shutdown();
	_PyDecideModuleProducer.shutdown();
	_PyGeneratorModuleProducer.shutdown();
	_PyModuleModuleProducer.shutdown();
	_PySwitchModuleProducer.shutdown();

	_initialized = false;
}

} // namespace pxl
