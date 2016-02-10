//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#ifndef INITIALIZE_ASTRO_HH_
#define INITIALIZE_ASTRO_HH_

#include "pxl/core/macros.hh"

namespace pxl
{
class PXL_DLL_EXPORT Astro
{
public:
	static void initialize();
	static void shutdown();

};

}

#endif /* INITIALIZE_ASTRO_HH_ */
