//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#ifndef PXL_MODULES_INPUT_MODULE_HH
#define PXL_MODULES_INPUT_MODULE_HH
#include "pxl/core/macros.hh"

#include "pxl/modules/Module.hh"
#include "pxl/core.hh"

namespace pxl
{
/**
 read PXL I/O files event by event
 */

class PXL_DLL_EXPORT InputModule: public Module
{
	InputFile *_InputFile;
	std::vector<std::string> _InputFileNames;
	Serializable* _Object;

	Source *_Source;

	int64_t _StartIndex;
	int64_t _EndIndex;

	int64_t _ObjectCount;

	Serializable *readNextObject();
public:

	InputModule();

	static const std::string& getStaticType();
	virtual const std::string& getType() const;
	bool isRunnable() const;

	void initialize () ;

	bool analyse(Sink *sink) ;

	void beginRun() ;
	void endRun() ;

	void openNextFile() ;
};

}

#endif /*PXL_MODULES_INPUT_MODULE_HH*/
