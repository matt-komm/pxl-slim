//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#include "pxl/core.hh"
#include "pxl/astro.hh"

namespace pxl
{

static bool _initialized = false;

static ObjectProducerTemplate<AstroBasicObject> _AstroBasicObjectProducer;
static ObjectProducerTemplate<AstroObject> _AstroObjectProducer;
static ObjectProducerTemplate<RegionOfInterest> _RegionOfInterestProducer;
static ObjectProducerTemplate<UHECR> _UHECRProducer;
static ObjectProducerTemplate<UHECRSource> _UHECRSourceProducer;

void Astro::initialize()
{
	if (_initialized)
		return;

	_AstroBasicObjectProducer.initialize();
	_AstroObjectProducer.initialize();
	_RegionOfInterestProducer.initialize();
	_UHECRProducer.initialize();
	_UHECRSourceProducer.initialize();

	_initialized = true;
}

void Astro::shutdown()
{
	if (_initialized == false)
		return;

	_AstroBasicObjectProducer.shutdown();
	_AstroObjectProducer.shutdown();
	_RegionOfInterestProducer.shutdown();
	_UHECRProducer.shutdown();
	_UHECRSourceProducer.shutdown();

	_initialized = false;
}

} // namespace pxl
