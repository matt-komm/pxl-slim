//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2010 Martin Erdmann        -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#ifndef PXL_XML_USERRECORDHANDLER_HH
#define PXL_XML_USERRECORDHANDLER_HH

#include <string>
#include <vector>
#include "pxl/xml/XmlHandler.hh"
#include "pxl/core/UserRecord.hh"

namespace pxl
{

/// Parsing one user record entry
class PXL_DLL_EXPORT UserRecordEntryHandler: public XmlHandler
{
	std::string name;
	std::string type;
public:
	UserRecordEntryHandler()
	{
	}

	void initialize(const char **attributes);

	const std::string& getName()
	{
		return name;
	}
	const std::string& getValue()
	{
		return data;
	}
	const std::string& getType()
	{
		return type;
	}

};

/// Class parsing user records - to be used as childparser for the
/// Module parser
class PXL_DLL_EXPORT UserRecordsXmlHandler: public XmlHandler
{
	UserRecords* _userRecord;
	UserRecordEntryHandler* _childHandler;
public:
	UserRecordsXmlHandler(UserRecords *userRecord) :
		_userRecord(userRecord), _childHandler(0)
	{
	}

	~UserRecordsXmlHandler();

	void doStartElement(const char *element, const char **attributes);

	void doEndChildHandler(XmlHandler *childHandler);

};

}
#endif /* MODULES_HH*/
