//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009 Martin Erdmann        -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#include "pxl/xml/AnalysisXmlExport.hh"
#include "pxl/xml/AnalysisXmlWriter.hh"
#include "pxl/xml/XmlStream.hh"
#include <stdlib.h>
#include <cstdio>

#define BUFFER_SIZE 4096


namespace pxl
{
static Logger logger("pxl::AnalysisXmlExport");

AnalysisXmlExport::AnalysisXmlExport() : _tmpFile(NULL)
{
}

AnalysisXmlExport::~AnalysisXmlExport()
{

}

bool AnalysisXmlExport::open(const std::string &filename)
{
	_targetFileName = filename;

	// Write into tmpfile to avoid loss of files if interrupted write
	if (_tmpFile)
		throw std::runtime_error("AnalysisXmlExport opened multiple times!");

	_tmpFile = new NamedTMPFile();

	file.open(_tmpFile->getName().c_str(), std::ios::out);
	if (file.bad())
	{
		fprintf(stderr, "Couldn't open tmp file\n  ");
		fputs(_tmpFile->getName().c_str(), stderr);
		return false;
	}
	file.imbue(std::locale("C"));
	file.precision(16);

	XmlStream stream(file);
	//stream.startElement("PXL");
    return true;
}

bool AnalysisXmlExport::close()
{
	XmlStream stream(file);
	//stream.endElement("PXL");
	file.close();

	FILE* source = fopen(_tmpFile->getName().c_str(),"rb");
	FILE* dest= fopen(_targetFileName.c_str(),"wb");
	if (!dest)
	{
		fprintf(stderr, "Couldn't open xml file:\n  ");
		fputs(_targetFileName.c_str(), stderr);
		delete _tmpFile;
		return false;
	}

	char buffer[BUFFER_SIZE];
	size_t size;
	while ( (size = fread(buffer, 1, BUFFER_SIZE, source)) ) 
	{
		fwrite(buffer, 1, size, dest);
  }
	fclose(source);
	fclose(dest);
	
	delete _tmpFile;
	return true;
}

void AnalysisXmlExport::writeObject(Analysis *object)
{
	XmlStream stream(file);
	AnalysisXmlWriter writer;
	writer.writeObject(object, stream);
}

} // namespace pxl
