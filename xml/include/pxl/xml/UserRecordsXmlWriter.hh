//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009 Martin Erdmann        -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#ifndef USERRECORDXMLWRITER_HH_
#define USERRECORDXMLWRITER_HH_

#include "XmlStream.hh"
#include "pxl/core/logging.hh"
#include "pxl/core/UserRecord.hh"
#include "pxl/core/Variant.hh"

#include <sstream>

namespace pxl {

//static Logger logger("pxl::UserRecordsXmlWriter");
class PXL_DLL_EXPORT UserRecordsXmlWriter
{
public:
	void writeObject(const UserRecords& object, XmlStream &stream) const;
	};

}
#endif /* USERRECORDXMLWRITER_HH_*/
