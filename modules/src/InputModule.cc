//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//		  http://pxl.sourceforge.net	  -
// Copyright (C) 2009-2010 Martin Erdmann		-
//			   RWTH Aachen, Germany	   -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#include "pxl/modules/InputModule.hh"
#include "pxl/modules/ModuleFactory.hh"
#include "pxl/core/logging.hh"
#include "pxl/core/macros.hh"

#include <memory>

namespace pxl
{
static Logger logger("pxl::InputModule");

InputModule::InputModule() :
	Module(), _InputFile(0), _Object(0), _Source(0), _StartIndex(0), _EndIndex(-1),
			_ObjectCount(0)
{
}

void InputModule::initialize()
{
	addSource("out", "All Objects from the input file.");

	addOption("start", "Number of events to be skipped at beginning of file(s)", int64_t(0));
	addOption("end", "Last event to read (inclusive; set to negative value to read until the end of the file)", int64_t(-1));
	addOption("File names", "File names which are sequentially read",
			std::vector<std::string>(), OptionDescription::USAGE_FILE_OPEN);
	Module::initialize();
}

const std::string& InputModule::getStaticType()
{
	static const std::string name("File Input");
	return name;
}

const std::string& InputModule::getType() const
{
	return getStaticType();
}

bool InputModule::isRunnable() const
{
	return true;
}

bool InputModule::analyse(Sink *sink)
{
	safe_delete(_Object);

	if (_EndIndex >= 0 && _ObjectCount >= _EndIndex - _StartIndex)
		return false;

	if (_InputFile == 0)
	{
		logger(LOG_LEVEL_DEBUG, getName(), "Input file not open");
		openNextFile();
		if (_InputFile == 0)
		{
			logger(LOG_LEVEL_INFO, getName(), " :  read ", _ObjectCount,
					" Objects");
			return false;
		}
	}
	bool eventsLeft = true;
	while (eventsLeft)
	{
		_Object = _InputFile->readNextObject();

		if (_Object)
		{
			logger(LOG_LEVEL_DEBUG, getName(), "Read next object successful");
			_ObjectCount++;
			_Source->setTargets(_Object);
			return _Source->processTargets();
		}
		else
		{
			logger(LOG_LEVEL_DEBUG, getName(), "End of file or read next object not working properly");
			openNextFile();
			if (_InputFile == 0)
			{
				logger(LOG_LEVEL_INFO, getName(), " :  read ", _ObjectCount,
						" Objects");
				return false;
			}
		}
	}
	return false;
}

void InputModule::openNextFile()
{

	safe_delete(_InputFile);
	if (_InputFileNames.size() <= 0)
	{
		return;
	}

	std::string filename = _InputFileNames.front();
	_InputFileNames.erase(_InputFileNames.begin());
	logger(LOG_LEVEL_INFO, "Open file:",filename);
	if (getAnalysis())
		filename = getAnalysis()->findFile(filename);

	if (filename.empty())
	{
		logger(LOG_LEVEL_WARNING, "Trying to open file with empty filename");
		return;
	}

	_InputFile = new InputFile();
	_InputFile->open(filename);

	if (_InputFile->good())
	{
		logger(LOG_LEVEL_INFO, getName(), " :  opened ", filename);
	}
	else
	{
		safe_delete(_InputFile);
		logger(LOG_LEVEL_ERROR, getName(), "File", filename, "not found!");
	}
}

void InputModule::beginRun()
{
	_ObjectCount = 0;
	logger(LOG_LEVEL_DEBUG, "InputModule: beginRun()");
	getOption("start", _StartIndex);
	getOption("end", _EndIndex);
	getOption("File names", _InputFileNames);
	openNextFile();

	if (_InputFile)
	{
		if (_StartIndex > 0)
		{
            int64_t offset = _StartIndex;
			logger(LOG_LEVEL_DEBUG, getName(), " :  skipping ", offset,
					" Objects");
			_InputFile->seekToObject(offset);
			offset -= _InputFile->objectCount();
			while (offset > 0 && _InputFileNames.size() > 0)
			{
				openNextFile();
				_InputFile->seekToObject(offset);
				offset -= _InputFile->objectCount();
			}
		}
	}

	_Source = getSource("out");
	
	logger(LOG_LEVEL_DEBUG,  "InputModule: beginRun done");
}

void InputModule::endRun()
{
	safe_delete(_Object);
	safe_delete(_InputFile);

	logger(LOG_LEVEL_INFO, getName(), ": read",  _ObjectCount, "objects");
}

} // namespace pxl
