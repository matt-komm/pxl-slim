//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#ifndef PXL_PARTICLE_SCRIPT_FILTER_HH
#define PXL_PARTICLE_SCRIPT_FILTER_HH
#include "pxl/core/macros.hh"

#include "pxl/hep/Particle.hh"
#include "pxl/scripting/DefaultScriptFilter.hh"

namespace pxl {
/**
 a python script for analyzing and filtering of pxl::Particle
 */

class PXL_DLL_EXPORT ParticleScriptFilter : public DefaultScriptFilter
{
	PyObject *_FilterFunction;
public:
	ParticleScriptFilter();
	ParticleScriptFilter(pxl::Particle *object);

	~ParticleScriptFilter();

	double evaluate(pxl::Particle *object);
};

} // namespace pxl

#endif // PXL_PARTICLE_SCRIPT_FILTER_HH
