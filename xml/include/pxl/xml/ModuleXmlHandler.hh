//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2010 Martin Erdmann        -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#ifndef PXL_XML_IMPL_MODULES_HH
#define PXL_XML_IMPL_MODULES_HH

#include <string>
#include <vector>
#include "pxl/xml/XmlHandler.hh"
#include "pxl/xml/UserRecordsXmlHandler.hh"
#include "pxl/core/UserRecord.hh"
namespace pxl
{

struct Option
{
	std::string name;
	std::string type;
	std::string value;
};

/// Class parsing module options - to be used as childparser for the
/// Module parser
class PXL_DLL_EXPORT  ModuleOptionHandler: public XmlHandler
{
	private:
		Option option;
	public:
		void initialize(const char **attributes);
		void doEndElement(const char *chars);
		const Option getOption() const;
};


/// Parses modules in analysis xml description 
class PXL_DLL_EXPORT ModuleXmlHandler: public XmlHandler
{
	XmlHandler* _childHandler;
	std::string name;
	std::string type;
	std::string runIndex;
	std::string isEnabled;
	std::vector<Option> options;
	UserRecords _userRecord;

public:
	ModuleXmlHandler() : _childHandler(0)
	{
	}
	
	~ModuleXmlHandler();

	void initialize(const char **attributes);
	void doStartElement(const char *element, const char **attributes);
	void doEndChildHandler(XmlHandler *childHandler);
	/// Returns name of the module read in
	const std::string& getModuleName() const;
	/// Returns type of the module read in
	const std::string& getModuleType() const;
	/// Returns the run index of the module read in
	const std::string& getRunIndex() const;
	//returns enabled status of the module read in
	const std::string& getEnabledStatus() const;

	/// Returns the model options 
	const std::vector<Option>& getModuleOptions() const;
	const UserRecords& getUserRecord() const
	{
		return _userRecord;
	};

};


class PXL_DLL_EXPORT  ConnectionXmlHandler: public XmlHandler
{
	std::string sourceModuleName;
	std::string sourceName;
	std::string sinkModuleName;
	std::string sinkName;

public:
	ConnectionXmlHandler()
	{
	}

	void doStartElement(const char *element, const char **attributes);
	const std::string& getSourceModuleName() const;
	const std::string& getSourceName() const;
	const std::string& getSinkModuleName() const;
	const std::string& getSinkName() const;
};






}
#endif /* MODULES_HH*/
