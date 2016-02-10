//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#ifndef PXL_MODULES_OUTPUT_MODULE_HH
#define PXL_MODULES_OUTPUT_MODULE_HH
#include "pxl/core/macros.hh"

#include "pxl/modules/Module.hh"
#include "pxl/core.hh"

namespace pxl
{
/**
 write PXL I/O files
 */

class PXL_DLL_EXPORT OutputModule: public Module
{
	OutputFile *_OutputFile;
	std::string _OutputFileName;

	size_t _ObjectCount;
	int64_t _ObjectSectionCount;
	int64_t _eventsFileSection;

public:
	OutputModule();
	virtual ~OutputModule();

	static const std::string& getStaticType();
	virtual const std::string& getType() const ;
	bool isRunnable() const;
	void initialize() ;
	bool analyse(Sink *sink) ;

	void beginJob() ;
	void endJob() ;
};

} // namespace pxl

#endif /*PXL_MODULES_OUTPUT_MODULE_HH*/
