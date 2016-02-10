#include "pxl/modules/Module.hh"
#include "pxl/modules/ModuleFactory.hh"
#include "pxl/core.hh"
#include "pxl/core/logging.hh"
#include <iostream>


pxl::Logger logger("TestModule");
//export PXL_LOG_LEVEL=0, to see INFO and DEBUG 
//export PXL_LOG_LEVEL=2, to see INFO and no DEBUG

// our module inherits from pxl::Module
class TestModule: public pxl::Module
{
private:
	// we want to have one sink (input) and one source (output)
	pxl::Serializable* _Object;

public:

	// initialize the super class Module as well
	TestModule() : Module(), _Object(0)
	{
		addSource("output","Output");
	}

	~TestModule()
	{
	}

	// every Module needs a unique type
	static const std::string &getStaticType()
	{
		static std::string type ("TestModule");
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
		return true;
	}
	void beginJob() 
	{
		logger(pxl::LOG_LEVEL_INFO, "TestModule: Begin job Called");
	}

	bool analyse(pxl::Sink *sink) 
	{
		logger(pxl::LOG_LEVEL_INFO, "TestModule: Begin job Called");
		return false;
	}

	void destroy() 
	{
		// only we know how to delete this module
		delete this;
	}
};

static pxl::ModuleProducerTemplate<TestModule> _TestModuleProducer;

void initialize()
{
	_TestModuleProducer.initialize();
}

PXL_PLUGIN_INIT

