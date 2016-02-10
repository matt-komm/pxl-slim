//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a GPL-2 or later license  -
//-------------------------------------------

#include "pxl/core.hh"
#include "pxl/astro/Healpix.hh"
#include "pxl/healpix.hh"

namespace pxl
{

static bool _initialized = false;

static ObjectProducerTemplate<HealpixMap> _HealpixMapProducer;

void Healpix::initialize()
{
	if (_initialized)
		return;

	_HealpixMapProducer.initialize();

	_initialized = true;
}

void Healpix::shutdown()
{
	if (_initialized == false)
		return;

	_HealpixMapProducer.shutdown();

	_initialized = false;
}

} // namespace pxl
