//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------


#include "pxl/core.hh"
#include "pxl/hep.hh"
#include "pxl/astro.hh"
#include "pxl/modules.hh"
#include "pxl/scripting.hh"
#include "pxl/xml.hh"

#include "pxl/core/ConfigLoader.hh"
#include "simpleopt/SimpleOpt.h"
#include "simpleini/SimpleIni.h"


#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
	#define WIN32_EXTRA_LEAN
	#include <windows.h>
#endif

#include <string>
#include <stdlib.h>
#include <list>
#include <vector>
using namespace pxl;

static Logger logger("pxlrun");


// define option ids
enum { OPT_help, 
	 OPT_pxlVersion,
	OPT_getDataPath, OPT_getUserPluginPath, OPT_getDefaultPluginPath,
	OPT_prefix, OPT_incdir, OPT_libdir,
	OPT_changePath, OPT_loadDefaultPlugins,
	OPT_verbose, OPT_very_verbose, OPT_loadPluginsFromDirectory, OPT_modOpt,
	OPT_pythonpath, OPT_searchpath,
	OPT_useConfigFile, OPT_ignoreConfigFile
};

// define options
CSimpleOpt::SOption g_rgOptions[] = {
	{ OPT_help, "-h", SO_NONE},
	{ OPT_help, "--help", SO_NONE},
	{ OPT_pxlVersion, "--version", SO_NONE},
	{ OPT_pxlVersion, "-V", SO_NONE},
	{ OPT_getDataPath, "--getDataPath", SO_NONE},
	{ OPT_getUserPluginPath, "--getUserPluginPath", SO_NONE},
	{ OPT_getDefaultPluginPath, "--getDefaultPluginPath", SO_NONE},
	{ OPT_prefix, "--prefix", SO_NONE},
	{ OPT_incdir, "--incdir", SO_NONE},
	{ OPT_libdir, "--libdir", SO_NONE},

	{ OPT_changePath, "-p", SO_NONE},
	{ OPT_loadDefaultPlugins, "-n", SO_NONE},
	{ OPT_verbose, "-v", SO_NONE},
	{ OPT_very_verbose, "-vv", SO_NONE},
	{ OPT_verbose, "--verbose", SO_NONE},
	{ OPT_modOpt, "--set", SO_OPT},
	{ OPT_loadPluginsFromDirectory, "-L", SO_OPT},
	{ OPT_pythonpath, "-pp", SO_OPT},
	{ OPT_pythonpath, "--pythonpath", SO_OPT},
	{ OPT_searchpath, "-s", SO_OPT},
	{ OPT_searchpath, "--addSearchPath", SO_OPT},
	{ OPT_useConfigFile, "--useRC", SO_OPT},
	{ OPT_ignoreConfigFile, "--ignoreRC", SO_NONE},
	SO_END_OF_OPTIONS
};

void usage()
{
	std::cout <<
	"Usage: pxlrun [OPTIONS] [FILE(s)]\n" 
	<< std::endl 
	<< "pxlrun can be used to execute pxl analysis stored in an xml file or to get information on the local pxl installation.\n"
	<< std::endl
	<<"Options for analysis execution:\n"
	<<"  -p                         change to the path of the input file before execution\n"
	<<"  -n                         do not load plugins from default directories,\n"
	<<"  -L=PATH                    search in PATH for plugins\n"
	<<"  --set=MODULE.OPTION=VALUE  Modify options of module \n"
	<<"  --pythonpath, -pp=PATH     add module path PATH to python sys.path\n"
	<<"  --addSearchPath, -s=PATH   add path PATH to analysis search path for files\n"
	<<"  --ignoreRC                 ignores the default configuration in $HOME/.pxlrc\n"
	<<"  --useRC=FILE               uses configuration from FILE instead of $HOME/.pxlrc\n"
	<<"  --verbose, -v              Verbose: print more messages\n"
	<< std::endl
	<<"Options to access information about the pxl installation:\n"
	<<"  --version, -V              print the pxl version\n"
	<<"  --getDataPath              print the path to pxl data\n"
	<<"  --getUserPluginPath        print the path to the user plugins\n"
	<<"  --getDefaultPluginPath     print the path to the default plugins\n"
	<<"  --prefix                   show PXL installation prefix \n"
	<<"  --incdir                   print the path of the header directory\n"
	<<"  --libdir                   print the path of the library directory\n"
	<<"  --help, -h                 Print this help message\n"
	<< std::endl;
}

struct ModuleOption
{
	std::string moduleName;
	std::string optionName;
	std::string optionValue;
};


int main(int argc, char **argv)
{
	//
	// Do not produce any output in any case before the options are
	// parsed, as this is used to return information about the
	// installation which will be parsed by other programs!
	//
	bool changePath = false;
	bool loadDefaultPlugins = true;

	bool ignoreRC = false;

	//the list of optionss for modlues
	std::vector<ModuleOption> moduleOptionList;

	// Additional directories to be used with analysis::findFile
	std::vector<std::string> analysisSearchPaths;
	
	std::vector<std::string> pythonModulePaths;

	//loop over cli arguments
	CSimpleOpt args(argc, argv, g_rgOptions, SO_O_NOERR | SO_O_EXACT);
	
	while (args.Next())
	{
	if (args.OptionId() == OPT_help)
		{
			usage();
			return 0;
		}
		else if (args.OptionId() == OPT_pxlVersion)
		{
			std::cout << "pxl: " << PXL_VERSION << std::endl << std::endl;
			return 0;
		}
		else if (args.OptionId() == OPT_getDataPath)
		{
			std::cout << PXL_DATA_PATH << std::endl;
			return 0;
		}
		else if (args.OptionId() == OPT_getUserPluginPath)
		{
			std::cout << PluginManager::getUserPluginDirectory() << std::endl;
			return 0;
		}
		else if (args.OptionId() == OPT_getDefaultPluginPath)
		{
			std::cout << PluginManager::getDefaultPluginDirectory() << std::endl;
			return 0;
		}
		else if (args.OptionId() == OPT_prefix)
		{
			std::cout << PXL_PREFIX << std::endl;
			return 0;
		}
		else if (args.OptionId() == OPT_incdir)
		{
			std::cout << PXL_INCDIR<< std::endl;
			return 0;
		}
		else if (args.OptionId() == OPT_libdir)
		{
			std::cout << PXL_LIBDIR << std::endl;
			return 0;
		}
		else if (args.OptionId() == OPT_changePath)
		{
			changePath = true;
		}
		else if (args.OptionId() == OPT_loadDefaultPlugins)
		{
			loadDefaultPlugins = false;
		}
		else if (args.OptionId() == OPT_loadPluginsFromDirectory)
		{
			PluginManager::instance().loadPluginsFromDirectory(args.OptionArg());
		}
		else if (args.OptionId() == OPT_modOpt)
		{
			//check if sane option, and put to option list.
			std::string setting = args.OptionArg();
			ModuleOption moduleOption;
			int pos;

			pos = setting.find(".");
			if (pos==-1)
			{
				throw std::runtime_error(std::string("Syntax Error in Setting") + args.OptionArg());
			}
			moduleOption.moduleName = setting.substr(0,pos);
			setting.erase(0,pos+1);

			pos = setting.find("=");
			if (pos==-1)
			{
				throw std::runtime_error(std::string("Syntax Error in Setting") + args.OptionArg());
			}
			moduleOption.optionName = setting.substr(0,pos);
			setting.erase(0,pos+1);

			moduleOption.optionValue = setting;
			logger(LOG_LEVEL_INFO,"Overide Option",moduleOption.moduleName,moduleOption.optionName, moduleOption.optionValue);
			moduleOptionList.push_back(moduleOption);
		}
		else if (args.OptionId() == OPT_verbose)
		{
			// Verbose (level info)
			pxl::LogDispatcher::instance().enableConsoleLogHandler(pxl::LOG_LEVEL_INFO);
		}
		else if (args.OptionId() == OPT_very_verbose)
		{
			// Very verbose (level debug)
			pxl::LogDispatcher::instance().enableConsoleLogHandler(pxl::LOG_LEVEL_DEBUG);
		}
		else if(args.OptionId() == OPT_pythonpath)
		{
			pythonModulePaths.push_back(args.OptionArg());
		}
		else if (args.OptionId() == OPT_searchpath)
		{
			analysisSearchPaths.push_back(args.OptionArg());
		}
		else if (args.OptionId() == OPT_ignoreConfigFile)
			{
			ignoreRC=true;
		}
		else if (args.OptionId() == OPT_useConfigFile)
		{
      logger(LOG_LEVEL_WARNING, "Not implemented yet!");
		}
		else
		{
			logger(LOG_LEVEL_ERROR, "Invalid Argument: ",args.OptionText());
			return -1;
		}
	}
	const char *hostname = getenv("HOSTNAME");
	if (hostname)
		logger(LOG_LEVEL_INFO, "started on host '", hostname, "'");
	else
		logger(LOG_LEVEL_INFO, "started");


  pxl::Core::initialize();
	pxl::Hep::initialize();
	pxl::Astro::initialize();


	if (args.FileCount()==0)
	{
		logger(LOG_LEVEL_ERROR,"No analysis file provided!");
		usage();
		return -1;
	}
	logger(LOG_LEVEL_DEBUG, "Found", args.FileCount(), "analysis files:");

	//
	// loading and processing the inifile
	//
	if (!ignoreRC)
	{
    loadDefaultConfigurations();
	} // ignoreRC

	
  if (loadDefaultPlugins)
		PluginManager::instance().loadPlugins();

	int returnValue=0;

	for (size_t inputFileNumber = 0; inputFileNumber < args.FileCount(); inputFileNumber++)
	{
		std::vector<Module*> _defectiveModules;
		Analysis analysis;
		
		try
		{
			logger(LOG_LEVEL_DEBUG,"Starting Analysis Import");
			AnalysisXmlImport analysisImporter;
			std::string filename = clearPathName(args.File(inputFileNumber));
			std::string path = getParentDirectory(filename);
			analysis.addSearchPath(path);
			if (changePath)
				analysis.setOutputPath(path);

			logger(LOG_LEVEL_DEBUG,"Importing: ", filename);
			analysisImporter.open(filename);
			_defectiveModules = analysisImporter.parseInto(&analysis);
			analysisImporter.close();
			// loop over module options and set options
			logger(LOG_LEVEL_DEBUG,"Overide Options");
			for(std::vector<ModuleOption>::iterator iter = moduleOptionList.begin();
					iter!=moduleOptionList.end(); ++iter)
			{
				//logger(LOG_LEVEL_WARNING,"Overide Options Currently Disabled!");
				analysis.setModuleOption((*iter).moduleName, (*iter).optionName, (*iter).optionValue);
			}

			logger(LOG_LEVEL_DEBUG,"Start Analysis");
			analysis.run();
			logger(LOG_LEVEL_DEBUG,"Analysis Finished, shutting down");
			if (_defectiveModules.size()!=0)
			{
				std::cerr << "pxlrun was informed of errors in " << _defectiveModules.size() << " module(s)." << std::endl;
				throw std::runtime_error("");
			}
		} catch (std::exception &e)
		{
			logger(LOG_LEVEL_ERROR, "*** Exception thrown during analysis execution - aborting *** \n", e.what() );
			std::cerr << "\npxlrun was informed of errors in the following " << _defectiveModules.size() << " module(s):" << std::endl;
			for (int i=0; i< _defectiveModules.size(); i++)
			{
				std::cerr << "[" << i+1 << "/" <<  _defectiveModules.size() << "]\t" << _defectiveModules[i]->getName() << " of type " << _defectiveModules[i]->getType() << std::endl;
				for (int j=0; j < _defectiveModules[i]->getStatusMessages().size(); j++)
				{
					std::cerr << " - " << _defectiveModules[i]->getStatusMessages()[j] << std::endl;
				}
			}
			analysis.shutdown();
			return 1;
		}
		analysis.shutdown();
	}
	return returnValue;
}

