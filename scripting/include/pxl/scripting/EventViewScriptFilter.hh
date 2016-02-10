//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#ifndef PXL_EVENT_VIEW_SCRIPT_FILTER_HH
#define PXL_EVENT_VIEW_SCRIPT_FILTER_HH

#include "pxl/core/macros.hh"
#include "pxl/hep/EventView.hh"
#include "pxl/scripting/DefaultScriptFilter.hh"

namespace pxl {
/**
 a python script for analyzing and filtering of pxl::EventView
 */

class PXL_DLL_EXPORT EventViewScriptFilter : public DefaultScriptFilter
{
	PyObject *_FilterFunction;
public:
	EventViewScriptFilter();
	EventViewScriptFilter(EventView *eventview);

	~EventViewScriptFilter();

	double evaluate(EventView *eventview);
};

} // namespace pxl

#endif // PXL_EVENT_VIEW_SCRIPT_FILTER_HH
