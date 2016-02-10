//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2010 Martin Erdmann        -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#ifndef PXL_PLUGINS_ANALYSIS_HH
#define PXL_PLUGINS_ANALYSIS_HH

#include "pxl/core/macros.hh"
#include "pxl/core/UserRecord.hh"
#include "pxl/core/Variant.hh"

#include <vector>

namespace pxl
{

// forward declaration
class Module;

//holding all necessary information about a connection 
struct Connection
{
	std::string sourceModuleName;
	std::string sourceName;
	std::string sinkModuleName;
	std::string sinkName;
};

typedef void (Module::*ModuleOnPublishFunction)(const Variant &);

/**
 Set of modules with additional information on how sources of different
 modules are connected to sinks of other modules.  The first module in
 the chain (module which either does not have any sinks or none of its
 sinks are connected to another source) has to provide container which are
 passed on through the chain. The chain ends for a specific container when
 it is not passed on to the next module at latest after it was processed by
 a module which has no sinks attached to its sources or does not have any
 sources.
 */
class PXL_DLL_EXPORT Analysis: public UserRecordHelper
{
	/// Storage of the modules. This vector is sorted before analysis
	/// execution by execution order.
	typedef std::vector<Module *> vector_t;
	typedef std::vector<Module *>::iterator iterator_t;
	vector_t _modules;
	std::vector<std::string> _searchPaths;
	std::string _outputPath;

	/// Conveniance storage for the connections
	std::vector<Connection *> _connections;

	//void loadFromElement(TiXmlElement* moduleelement)
	//		;

	//These are loops over the analysis chain, executing the individual
	//method of the Module
	void beginJob();
	void beginRun();
	void endRun();
	void endJob();

public:
	Analysis();
	~Analysis();

	/// cleans up the analysis before destruction
	void shutdown();
	/// sets a module Option
	void setModuleOption(const std::string& moduleName,
			const std::string& optionname, const std::string& value);
	/// Executes an previous load or defined analysis
	void run();

	// returns a vector of the modules in the analysis sorted in order of
	// execution
	const std::vector<Module *>& getModules() const;

	/// returns a vector of the module connections
	const std::vector<Connection *>& getConnections() const;

	/// Creates a module of given type, name, id and runIndex in the
	/// analysis
	Module* addModule(const char *moduleType, const char *moduleName,
			const char *runIndex);

	/// Adds an existing module to the analysis, and takes ownership of
	//the moduel by this
	void addModule(Module* module);

	/// Removes the module  and all connections to it from the analysis
	void removeModule(const char *moduleName);

	/// Creates a connection between the source and sink of the modules
	void connectModules(const char *sourceModuleName, const char *sourceName,
			const char *sinkModuleName, const char *sinkName);

	/// Removes the given connection
	void disconnectModules(const char *sourceModuleName, const char *sourceName,
			const char *sinkModuleName, const char *sinkName);

	/// Returns an interface to the given module
	Module* getModule(const std::string& name);

	/// Checks if module with name in analysis
	bool hasModule(const std::string& name);

	/// Sets the output path of the analysis.
	void setOutputPath(const std::string &path);
	void addSearchPath(const std::string &path);

	//load the default search paths from the pxl config
	void addDefaultSearchPaths();
	std::string findFile(const std::string &file);
	const std::string &getOutputPath() const;
	std::string getOutputFile(const std::string &file);

private:

	bool nextModules(long &runIndex, std::vector<Module *> &modules);
};

} //namespace pxl

#include "pxl/modules/Module.hh"

#endif //PXL_ANALYSIS_HH
