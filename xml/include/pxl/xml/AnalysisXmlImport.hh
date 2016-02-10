//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2010 Martin Erdmann        -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#ifndef ANALYSISXMLIMPORT_HH
#define ANALYSISXMLIMPORT_HH

#include <string>
#include <vector>
#include "pxl/modules/Analysis.hh"

typedef struct XML_ParserStruct *XML_Parser;

namespace pxl {

class PXL_DLL_EXPORT XmlHandler;

/// Providing methods to load modules and connections from a xml file
/// into an analysis
class PXL_DLL_EXPORT AnalysisXmlImport {
	XML_Parser parser;
	int status;
	std::vector<Module*> _defectiveModules;
	FILE *file;
	int depth;
	XmlHandler *_handler;
	Analysis* _analysis; // Pointer to the analysis the file is parsed into

public:
	AnalysisXmlImport();
	~AnalysisXmlImport();
	bool open(const std::string &filename);
	void close();
	//reads an xml file and adds modules and connections stored in it to
	//the analysis
	std::vector<Module*> parseInto(Analysis* analysis);
	void startElement(const char *element, const char **attributes);
	void endElement(const char *element);
	void startCData();
	void endCData();
	void dataElement(const char *data, int size);
};

} // namespace pxl

#endif /* ANALYSISXMLIMPORT_HH*/
