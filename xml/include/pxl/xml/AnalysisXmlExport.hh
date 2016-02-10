//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009 - 2010 Martin Erdmann -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#ifndef PXL_ANALYSIS_XML_XMLEXPORT_HH
#define PXL_ANALYSIS_XML_XMLEXPORT_HH

#include <string>
#include <fstream>
#include "pxl/modules/Analysis.hh"
#include "pxl/core/NamedTMPFile.hh"

namespace pxl
{

class Serializable;

class PXL_DLL_EXPORT AnalysisXmlExport
{
	std::ofstream file;
	int depth;
	std::string _targetFileName;
	pxl::NamedTMPFile *_tmpFile;

public:
	AnalysisXmlExport();

	~AnalysisXmlExport();

	/// The open method opens only an teporary file, to which the
	/// analysis is written. On close the tmpfile is copied to the final
	/// destination.
	bool open(const std::string &filename);
	bool close();

	void writeObject(Analysis *object);
};

} // namespace pxl

#endif /* ANALYSISXMLEXPORT_HH_ */
