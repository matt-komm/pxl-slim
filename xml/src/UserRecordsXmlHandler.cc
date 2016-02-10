//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2010 Martin Erdmann        -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#include "pxl/xml/UserRecordsXmlHandler.hh"
#include "pxl/core/functions.hh"
#include "pxl/core/logging.hh"
#include "pxl/core/Variant.hh"

using namespace pxl;
static Logger logger("pxl::UserRecordXmlHandler");

void UserRecordEntryHandler::initialize(const char **attributes)
{
	logger(LOG_LEVEL_DEBUG, "User Record Entry Initialized");
	name.append(findAttribute("name", attributes));
	type.append(findAttribute("type", attributes));
	logger(LOG_LEVEL_DEBUG, " * Found ", name, " ", type);
	XmlHandler::initialize(attributes);
}

UserRecordsXmlHandler::~UserRecordsXmlHandler()
{
	safe_delete (_childHandler);
}

void UserRecordsXmlHandler::doStartElement(const char *element,
		const char **attributes)
{
	if (strcmp(element, "entry") == 0)
	{
		logger(LOG_LEVEL_DEBUG, "New Entry Found");
		safe_delete (_childHandler);
		_childHandler = new UserRecordEntryHandler();
		setChildHandler(_childHandler);
		_childHandler->initialize(attributes);
	}
}

void UserRecordsXmlHandler::doEndChildHandler(XmlHandler *childHandler)
{
	// add Entry in UserRecord
	UserRecordEntryHandler *urh =
			dynamic_cast<UserRecordEntryHandler*> (childHandler);
	if (urh)
	{
		_userRecord->set(urh->getName(), Variant::fromString(urh->getValue(),
				Variant::toType(urh->getType())));
		logger(LOG_LEVEL_DEBUG, "Added Entry, ", urh->getName(), " ",
				_userRecord->get(urh->getName()), " wanted: ", urh->getValue());
	}

}

