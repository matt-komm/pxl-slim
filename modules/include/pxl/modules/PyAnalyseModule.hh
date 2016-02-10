//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#ifndef PXL_MODULES_PY_ANALYSE_MODULE_HH
#define PXL_MODULES_PY_ANALYSE_MODULE_HH
#include "pxl/core/macros.hh"

#include "pxl/modules/PyModule.hh"
#include <algorithm>
#include <memory>

namespace pxl {
/**
 module for executing python scripts
 */

class PXL_DLL_EXPORT PyAnalyseModule : public PyModule
{
public:
	static const std::string& getStaticType();
	virtual const std::string& getType() const;
	bool isRunnable() const;
	void initialize() ;
	bool analyse (Sink *sink) ;
};

} // namespace pxl

#endif // PXL_MODULES_PY_ANALYSE_MODULE_HH
