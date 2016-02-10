//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#ifndef PXL_MODULES_PY_GENERATOR_MODULE_HH
#define PXL_MODULES_PY_GENERATOR_MODULE_HH
#include "pxl/core/macros.hh"


#include "pxl/modules/PyModule.hh"
#include <algorithm>
#include <memory>

namespace pxl
{
/**
 module which allows to generate events for the module chain from python scripts
 */

class PXL_DLL_EXPORT PyGeneratorModule: public PyModule
{
	int64_t _EndIndex;
	int64_t _ObjectCount;

	PyObject *_GenerateFunction;

	Serializable* _Object;

public:
	PyGeneratorModule();

	static const std::string& getStaticType();
	virtual const std::string& getType() const;
	bool isRunnable() const;
	void initialize() ;

	bool analyse(Sink *sink) ;

	void beginJob() ;
	void endJob() ;
};

} // namespace pxl

#endif // PXL_MODULES_PY_GENERATOR_MODULE_HH
