//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#include "pxl/modules/ModuleFactory.hh"

namespace pxl
{
static Logger logger("pxl::ModuleFactory");

ModuleFactory& ModuleFactory::instance()
{
	static ModuleFactory f;
	return f;
}

ModuleFactory::ModuleFactory()
{

}

ModuleFactory::~ModuleFactory()
{
}

Module *ModuleFactory::createModule(const std::string& type) const
{
	std::map<std::string, const ModuleProducerInterface *>::const_iterator
			result;
	result = _Producers.find(type);
	if (result == _Producers.end())
		return 0;
	else
		return (*result).second->createModule();
}

void ModuleFactory::registerModuleProducer(
		const ModuleProducerInterface* producer, const std::string& type)
{
	logger(LOG_LEVEL_INFO, "register module producer", type);
	_Producers[type] = producer;
	_AvailableModules.push_back(type);
}

void ModuleFactory::unregisterModuleProducer(
		const ModuleProducerInterface* producer)
{

	std::map<std::string, const ModuleProducerInterface *>::iterator i;
	for (i = _Producers.begin(); i != _Producers.end(); i++)
	{
		if (i->second == producer)
		{
			logger(LOG_LEVEL_INFO, "unregister module producer for", i->first);

			// remove name		
			for (std::vector<std::string>::iterator iName =
					_AvailableModules.begin(); iName != _AvailableModules.end(); iName++)
			{
				if ((*iName) == i->first)
				{
					_AvailableModules.erase(iName);
					break;
				}
			}

			// remove producer
			_Producers.erase(i);

			return;
		}
	}
}

const std::vector<std::string> &ModuleFactory::getAvailableModules() const
{
	return _AvailableModules;
}

} // namespace pxl

