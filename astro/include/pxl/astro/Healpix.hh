//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a GPL-2 or later license  -
//-------------------------------------------

#ifndef INITIALIZE_HEALPIX_HH
#define INITIALIZE_HEALPIX_HH

#include "pxl/core/macros.hh"

namespace pxl
{
class PXL_DLL_EXPORT Healpix
{
public:
	static void initialize();
	static void shutdown();

};

}

#endif /* INITIALIZE_HEALPIX_HH_*/
