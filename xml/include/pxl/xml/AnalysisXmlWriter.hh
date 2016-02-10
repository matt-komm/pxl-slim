//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009 Martin Erdmann        -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#ifndef ANALYSISXMLWRITER_HH_
#define ANALYSISXMLWRITER_HH_

#include "XmlStream.hh"
#include "pxl/modules/Analysis.hh"
#include "pxl/core/logging.hh"

#include <iostream>

namespace pxl {


class PXL_DLL_EXPORT AnalysisXmlWriter
{
public:
	void writeObject(Analysis *object, XmlStream &stream) const;

};

}
#endif /* ANALYSISXMLWRITER_HH_*/
