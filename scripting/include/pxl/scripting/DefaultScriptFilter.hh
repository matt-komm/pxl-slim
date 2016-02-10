//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#ifndef PXL_DEFAULT_SCRIPT_HH
#define PXL_DEFAULT_SCRIPT_HH

#include <string>
#include <stdexcept>

#include "pxl/core/macros.hh"
#include "pxl/scripting/Script.hh"

namespace pxl {
/**
 a python script for analyzing and filtering the contents of an
 object inheriting from pxl::Serializable.
 */

class PXL_DLL_EXPORT DefaultScriptFilter : public Script
{
public:
	DefaultScriptFilter();
	virtual ~DefaultScriptFilter();

	bool create (const std::string& script, const std::string& filename);
	void destroy();

	void beginJob(const std::string &parameters);
	void beginRun();
	void endRun();
	void endJob();

private:

	PyObject* beginJobFunction;
	PyObject* beginRunFunction;
	PyObject* endRunFunction;
	PyObject* endJobFunction;
};

} // namespace pxl

#endif // PXL_DEFAULT_SCRIPT_HH

