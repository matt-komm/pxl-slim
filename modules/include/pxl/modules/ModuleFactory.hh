//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#ifndef PXL_MODULES_MODULE_FACTORY_HH
#define PXL_MODULES_MODULE_FACTORY_HH
#include "pxl/core/macros.hh"

#include <map>
#include <string>

#include "pxl/modules/Module.hh"
#include "pxl/core/logging.hh"

#include <map>

namespace pxl
{

class ModuleProducerInterface;

class PXL_DLL_EXPORT ModuleFactory
{
private:

	std::map<std::string, const ModuleProducerInterface *> _Producers;
	std::vector<std::string> _AvailableModules;

public:
	ModuleFactory();
	~ModuleFactory();

	static ModuleFactory& instance();

	Module *createModule(const std::string& type) const;
	const std::vector<std::string> &getAvailableModules() const;

	void registerModuleProducer(const ModuleProducerInterface* producer,
			const std::string& type);
	void unregisterModuleProducer(const ModuleProducerInterface* producer);
};

class ModuleProducerInterface
{
public:
	virtual ~ModuleProducerInterface()
	{
	}

	virtual Module *createModule() const = 0;
};

template<class T>
class ModuleProducerTemplate: public ModuleProducerInterface
{
public:

	void initialize()
	{
		ModuleFactory::instance().registerModuleProducer(this,
				T::getStaticType());
	}

	void shutdown()
	{
		ModuleFactory::instance().unregisterModuleProducer(this);
	}

	Module *createModule() const
	{
		return new T;
	}
};

} // namespace pxl

#endif // PXL_IO_OBJECT_FACTORY_HH
