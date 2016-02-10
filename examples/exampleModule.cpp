//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//          http://pxl.sourceforge.net      -
// Copyright (C) 2009-2010 Martin Erdmann        -
//               RWTH Aachen, Germany       -
// Contact: pxl-users@lists.sourceforge.net -
//-------------------------------------------

#include "pxl/core/macros.hh"
#include "pxl/core/PluginManager.hh"
#include "pxl/modules/Module.hh"
#include "pxl/modules/ModuleFactory.hh"

#include <iostream>

// our module inherits from pxl::Module
class ExampleModule: public pxl::Module
{
private:
	int64_t _Stats;
	int64_t _Count;

public:

	// initialize the super class Module as well
	ExampleModule() :
		Module()
	{
		// we need to tell the sink that this module should be
		// called when data was sent to it

		// set defaults for _Stats and _Count
		_Count = 0;
		// we want to have one option, with default value 100
		addOption("stats", "Print statistics every <stats> events.", int64_t(100));
		// we want to have one sink (input) and one source (output)
		addSink("input", "Input");
		addSource("output", "Output");
	}

	~ExampleModule()
	{
	}

	// every Module needs a unique type
	static const std::string &getStaticType()
	{
		static std::string type("Example Module");
		return type;
	}

	// static and dynamic methods are needed
	const std::string &getType() const
	{
		return getStaticType();
	}

	bool isRunnable() const
	{
		// this module does not proide events, so return false
		return false;
	}
	void beginJob() throw (std::runtime_error)
	{
		// copy option to local variables to avoid map search due to
		// performance
		getOption("stats", _Stats);
	}

	bool analyse(pxl::Sink *sink) throw (std::runtime_error)
	{
		_Count++;

		// output basic statistics
		if ((_Count > 0) && (_Count % _Stats == 0))
		{
			std::cout << "Example Module: " << _Count << " events analysed"
					<< std::endl;
		}

		// simply pass the events from input to output
		getSource("output")->setTargets(getSink("input")->get());
		return getSource("output")->processTargets();
	}

	void destroy() throw (std::runtime_error)
	{
		// only we know how to delete this module
		delete this;
	}
};

PXL_MODULE_INIT(ExampleModule)
PXL_PLUGIN_INIT

