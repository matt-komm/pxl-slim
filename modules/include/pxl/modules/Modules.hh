//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#ifndef PXL_MODULES_CLASS_HH
#define PXL_MODULES_CLASS_HH

#include "pxl/core/macros.hh"

namespace pxl
{

class PXL_DLL_EXPORT Modules
{
public:
	static void initialize();
	static void shutdown();
};

}

#endif // PXL_MODULES_CLASS_HH
