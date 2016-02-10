//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#include "pxl/modules/OutputModule.hh"
#include "pxl/modules/ModuleFactory.hh"
#include "pxl/core/logging.hh"
#include "pxl/core/functions.hh"

namespace pxl
{
static Logger logger("pxl::OutputModule");

OutputModule::OutputModule() :
	Module(), _ObjectCount(0), _ObjectSectionCount(0), _eventsFileSection(1000)
{

}

void OutputModule::initialize() 
{
	addSink("in", "write all Objects to file");
	addSource("out", "all Objects from the input.");

	addOption("filename", "Filename of the pxlio-File to write.",
			std::string(), OptionDescription::USAGE_FILE_SAVE);
	addOption("Buffer size", "Size of the I/O buffer.",
			int64_t(1048576L), OptionDescription::USAGE_TEXT);
	addOption("# Objects/file section", "Objects per file section.",
			int64_t(1000L), OptionDescription::USAGE_TEXT);
	addOption("Compression level", "Compression level (0-9).",
			int64_t(1), OptionDescription::USAGE_TEXT);
	Module::initialize();
}

OutputModule::~OutputModule()
{
	shutdown();
}

const std::string& OutputModule::getStaticType()
{
	static const std::string name("File Output");
	return name;
}

const std::string& OutputModule::getType() const 
{
	return getStaticType();
}

bool OutputModule::isRunnable() const
{
	return false;
}
bool OutputModule::analyse(Sink *sink) 
{
	Serializable *s = sink->get();

	if (s == 0)
		return false;

	_OutputFile->streamObject(s);
	if (_ObjectSectionCount >= _eventsFileSection)
	{
		_OutputFile->finish();
		_ObjectSectionCount = 0;
	}

	_ObjectCount++;
	_ObjectSectionCount++;

	Source *src = getSource("out");
	if (src->getSink() == 0)
	{
		return true;
	}
	else
	{
		src->setTargets(s);
		return src->processTargets();
	}
}

void OutputModule::beginJob() 
{
	logger(LOG_LEVEL_DEBUG, getName(),"beginJob started");
	int64_t buffer = 1024 * 1024;
	getOption("filename", _OutputFileName);
	getOption("Buffer size", buffer);
	getOption("# Objects/file section", _eventsFileSection);

	if (_OutputFileName.empty())
	{
		logger(LOG_LEVEL_ERROR, getName(),"Empty filename option.");
		throw std::runtime_error("Empty filename option in OutputModule " + getName());
	}
	std::string _expOutputFileName = expandEnvironmentVariables(_OutputFileName);

	if (getAnalysis())
		_OutputFileName = getAnalysis()->getOutputFile(_expOutputFileName);

	_OutputFile = new OutputFile(_OutputFileName, buffer, _eventsFileSection);

	int64_t comp = 1;
	getOption("Compression level", comp);
	if (comp < 0)
	{
		logger(LOG_LEVEL_WARNING, getName(),
				"invalid compression level, setting to 1");
		comp = 1;
	}
	else if (comp > 9)
	{
		logger(LOG_LEVEL_WARNING, getName(),
				"invalid compression level, setting to 9");
		comp = 9;
	}

	if (_OutputFile)
		_OutputFile->setCompressionMode((int) comp);

}

void OutputModule::endJob() 
{
	if (_OutputFile)
	{
		logger(LOG_LEVEL_INFO, getName(), ": wrote ", _ObjectCount, " Objects.");
		delete _OutputFile;
		_OutputFile = 0;
	}
}

} // namespace pxl

