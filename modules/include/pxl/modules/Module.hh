//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#ifndef PXL_MODULES_MODULE_H
#define PXL_MODULES_MODULE_H
#include "pxl/core/macros.hh"


#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <algorithm>

#include "pxl/core/Serializable.hh"
#include "pxl/core/UserRecord.hh"
#include "pxl/core/Variant.hh"
#include "pxl/modules/Analysis.hh"

namespace pxl
{

class Source;
class Module;
class Analysis;

struct SinkDescription
{
	std::string name;
	std::string description;
};

struct SourceDescription
{
	std::string name;
	std::string description;
};

struct OptionDescription
{
	enum Type
	{
		TYPE_STRING, TYPE_STRING_VECTOR, TYPE_DOUBLE, TYPE_LONG, TYPE_BOOLEAN
	};

	enum Usage
	{
		USAGE_UNKNOWN,
		USAGE_FILE_OPEN,
		USAGE_FILE_SAVE,
		USAGE_TEXT,
		USAGE_TEXT_MULTILINE,
		USAGE_TEXT_SELECTION
	};

	std::string name;
	std::string description;
	Type type;
	Usage usage;
};

class PXL_DLL_EXPORT Sink
{
	Serializable *_Serializable;
	Module *_Module;
	std::vector<Source *> _Sources;

public:
	Sink() :
		_Serializable(0), _Module(0)
	{

	}

	virtual ~Sink();

	void set(Serializable *event)
	{
		_Serializable = event;
	}

	Serializable *get()
	{
		return _Serializable;
	}

	Module *getModule()
	{
		return _Module;
	}

	void setModule(Module *module)
	{
		_Module = module;
	}

	void addSource(Source *source)
	{
		if (std::find(_Sources.begin(), _Sources.end(), source)
				== _Sources.end())
			_Sources.push_back(source);
	}

	void removeSource(Source *source)
	{
		_Sources.erase(std::remove(_Sources.begin(), _Sources.end(), source),
				_Sources.end());
	}
};

class PXL_DLL_EXPORT Source
{
	Sink * _Sink;

public:

	Source() :
		_Sink(0)
	{
	}

	virtual ~Source()
	{
		disconnect();
	}

	void connect(Sink *sink);

	void disconnect();

	Sink *getSink()
	{
		return _Sink;
	}

	void setTargets(Serializable *event);

	bool processTargets();
};

class PXL_DLL_EXPORT Module: public UserRecordHelper
{
private:
	std::string _name;
	bool _enabled;
	int64_t _runIndex;
	Analysis *_analysis;
	std::vector<pxl::SinkDescription> _sinkDescriptions;
	std::vector<pxl::SourceDescription> _sourceDescriptions;
	std::vector<pxl::OptionDescription> _optionDescriptions;

protected:
	std::map<std::string, Sink> _sinks;
	std::map<std::string, Source> _sources;
	std::map<std::string, Variant> _optionValues;
	std::vector<std::string> _statusMessages;

public:

	Module();

	virtual ~Module();

	void setName(const std::string &name);

	void setAnalysis(Analysis* analysis);

	const std::string& getName() const;

	virtual const std::string& getVersion() const;

	/// Returns enabled state of the module - if false, the module won't
	/// be executed in an analysis.
	bool isEnabled() const;

	/// Set to false to disable the module in an analysis. 
	void setEnabled(bool enabled);

	int64_t getRunIndex() const;

	void setRunIndex(int64_t index);
	Analysis *getAnalysis();

	virtual const std::string& getType() const;

	virtual void beginJob();

	virtual void beginRun();

	virtual bool analyse(Sink *sink) = 0;

	virtual void endRun();

	virtual void endJob();

	virtual Sink *getSink(const std::string &name) ;

	virtual const std::vector<pxl::SinkDescription> &getSinkDescriptions() const;

	virtual const std::vector<pxl::SourceDescription>
	&getSourceDescriptions() const;

	virtual Source *getSource(const std::string &name);

	Source* addSource(const std::string &name, const std::string &description);

	Sink* addSink(const std::string &name, const std::string &description);

	virtual const std::vector<pxl::OptionDescription>
	&getOptionDescriptions() const;

	void addOption(const std::string& name, const std::string& description,
			double defaultValue, OptionDescription::Usage usage = OptionDescription::USAGE_UNKNOWN);

	void addOption(const std::string& name, const std::string& description,
			int64_t defaultValue, OptionDescription::Usage usage = OptionDescription::USAGE_UNKNOWN);

	void addOption(const std::string& name, const std::string& description,
			const std::string& defaultValue, OptionDescription::Usage usage = OptionDescription::USAGE_UNKNOWN);

	void addOption(const std::string& name, const std::string& description,
			const char * defaultValue, OptionDescription::Usage usage = OptionDescription::USAGE_UNKNOWN);

	void addOption(const std::string& name, const std::string& description,
			bool defaultValue, OptionDescription::Usage usage = OptionDescription::USAGE_UNKNOWN);

	void addOption(const std::string& name, const std::string& description,
			const std::vector<std::string>& defaultValue, OptionDescription::Usage usage = OptionDescription::USAGE_UNKNOWN);

	virtual const OptionDescription &getOptionDescription(
			const std::string &name) const ;

	virtual void getOption(const std::string &name, std::string &value) const;
	virtual void getOption(const std::string &name, std::vector<std::string> &value) const;
	virtual void getOption(const std::string &name, double &value) const;
	virtual void getOption(const std::string &name, int64_t &value) const;
	virtual void getOption(const std::string &name, bool &value) const;

	virtual void setOption(const std::string &name, const std::string &value);
	virtual void setOption(const std::string &name, const std::vector<std::string> &value);
	virtual void setOption(const std::string &name, const double &value);
	virtual void setOption(const std::string &name, const int64_t &value);
	virtual void setOption(const std::string &name, const bool &value);

	virtual bool hasOption(const std::string &name) const;
	virtual bool hasOption(const std::string &name, OptionDescription::Type type);
	virtual bool hasSink(const std::string &name);
	virtual bool hasSource(const std::string &name);

	virtual void initialize() ;
	virtual void shutdown() ;
	virtual void destroy();
	virtual void reload();
	virtual void onPublish(size_t messageid, const pxl::Variant &v);

	/// Returns true if Module starts the analysis chain by emitting objects
	virtual bool isRunnable() const;

	///Returns the status messages of the module
	const std::vector<std::string>& getStatusMessages() const
	{
		return _statusMessages;
	}

	///Sets the current status message of the module
	void addStatusMessage(const std::string& message)
	{
		_statusMessages.push_back(message);
	}

	void clearStatusMessages()
	{
		_statusMessages.clear();
	}


protected:
	void syncDescriptions();
	void clearDescriptions();
};

} // namespace pxl

#define PXL_MODULE_INIT(MODULENAME)\
	static pxl::ModuleProducerTemplate<MODULENAME> _ ## MODULENAME ## Producer; \
	void initialize()\
	{\
	_ ## MODULENAME ## Producer.initialize();\
	}\

#endif // PXL_MODULES_MODULE_H
