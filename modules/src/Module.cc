//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2010 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#include "pxl/core/logging.hh"
#include "pxl/modules/Module.hh"
#include "pxl/modules/Analysis.hh"
#include <map>

namespace pxl
{

static Logger logger("pxl::Module");

void Source::connect(Sink *sink)
{
	if (sink == 0)
		return;

	if (_Sink != 0)
		throw std::runtime_error("Source already connected!");

	sink->addSource(this);
	_Sink = sink;
}

void Source::disconnect()
{
	if (_Sink)
	{
		_Sink->removeSource(this);
		_Sink = 0;
	}
}

void Source::setTargets(Serializable *event)
{
	if (_Sink)
		_Sink->set(event);
}

bool Source::processTargets()
{
	if (_Sink == 0)
		return true;

	Module *module = _Sink->getModule();
	if (module == 0)
		return true;

	if (module->isEnabled() == false)
	{
		return true;
	}

	return module->analyse(_Sink);
}

Sink::~Sink()
{
	while (_Sources.size() > 0)
	{
		_Sources.front()->disconnect();
	}
}

Module::Module() : _name("default"), _enabled(true), _runIndex(0), _analysis(0)
{
#ifdef __DATE__
	setUserRecord("Compilation Date", __DATE__);
#endif
#ifdef __TIME__
	setUserRecord("Compilation Time", __TIME__);
#endif
}

Module::~Module()
{
	shutdown();
}

const std::string& Module::getName() const
{
	return _name;
}

const std::string& Module::getVersion() const
{
	static std::string _defaultVersion("0.0.0");
	return _defaultVersion;
}

bool Module::isEnabled() const
{
	return _enabled;
}

void Module::setEnabled(bool enabled)
{
	_enabled = enabled;
}

int64_t Module::getRunIndex() const
{
	return _runIndex;
}

void Module::setRunIndex(int64_t index)
{
	_runIndex = index;
}

Analysis *Module::getAnalysis()
{
	return _analysis;
}

const std::string& Module::getType() const
{
	throw std::runtime_error("No type set.");
}

void Module::beginJob()
{
	logger(LOG_LEVEL_DEBUG, "InputModule", this->getName(),
			"default beginJob() !");
}

void Module::beginRun()
{
	logger(LOG_LEVEL_DEBUG, "InputModule", this->getName(),
			"default beginRun() !");
}

void Module::endRun()
{
	logger(LOG_LEVEL_DEBUG, "InputModule", this->getName(),
			"default endRun() !");
}

void Module::endJob()
{
	logger(LOG_LEVEL_DEBUG, "InputModule", this->getName(),
			"default endJob() !");
}

Sink *Module::getSink(const std::string &name)
{
	std::map<std::string, Sink>::iterator it = _sinks.find(name);
	if (it != _sinks.end())
	{
		return &(it->second);
	}
	else
	{
		throw std::runtime_error("Sink not found");
	}
}

const std::vector<pxl::SinkDescription> &Module::getSinkDescriptions() const
{
	// again, all modules have the same sinks, so make it static
	return _sinkDescriptions;
}

const std::vector<pxl::SourceDescription> &Module::getSourceDescriptions() const
{
	// again, all modules have the same sinks, so make it static
	return _sourceDescriptions;
}

Source *Module::getSource(const std::string &name)
{
	std::map<std::string, Source>::iterator it = _sources.find(name);
	if (it != _sources.end())
	{
		return &(it->second);
	}
	else
	{
		throw std::runtime_error("Source named '" + name + "' not found");
	}
}

Source *Module::addSource(const std::string &name,
		const std::string &description)
{
	SourceDescription sd;
	sd.name = name;
	sd.description = description;
	_sourceDescriptions.push_back(sd);

	return &_sources[name];
}

Sink* Module::addSink(const std::string &name, const std::string &description)
{
	SinkDescription sd;
	sd.name = name;
	sd.description = description;
	_sinkDescriptions.push_back(sd);

	Sink &s = _sinks[name];
	s.setModule(this);
	return &s;
}

const std::vector<pxl::OptionDescription> &Module::getOptionDescriptions() const
{
	return _optionDescriptions;
}

void Module::addOption(const std::string& name, const std::string& description,
		double defaultValue, OptionDescription::Usage usage)
{
	if (hasOption(name))
		return;

	OptionDescription o;
	o.name = name;
	o.description = description;
	o.type = OptionDescription::TYPE_DOUBLE;
	o.usage = usage;
	_optionDescriptions.push_back(o);

	if (_optionValues.find(name) == _optionValues.end())
		_optionValues[name] = defaultValue;
}

void Module::addOption(const std::string& name, const std::string& description,
		int64_t defaultValue, OptionDescription::Usage usage)
{
	if (hasOption(name))
		return;
	OptionDescription o;
	o.name = name;
	o.description = description;
	o.type = OptionDescription::TYPE_LONG;
	o.usage = usage;
	_optionDescriptions.push_back(o);

	if (_optionValues.find(name) == _optionValues.end())
		_optionValues[name] = defaultValue;
}

void Module::addOption(const std::string& name, const std::string& description,
		const std::string& defaultValue, OptionDescription::Usage usage)
{
	if (hasOption(name))
		return;
	OptionDescription o;
	o.name = name;
	o.description = description;
	o.type = OptionDescription::TYPE_STRING;
	o.usage = usage;
	_optionDescriptions.push_back(o);
	if (_optionValues.find(name) == _optionValues.end())
		_optionValues[name] = defaultValue;
}

void Module::addOption(const std::string& name, const std::string& description,
		const char *defaultValue, OptionDescription::Usage usage)
{
	if (hasOption(name))
		return;
	addOption(name, description, std::string(defaultValue), usage);
}

void Module::addOption(const std::string& name, const std::string& description,
		bool defaultValue, OptionDescription::Usage usage)
{
	if (hasOption(name))
		return;
	OptionDescription o;
	o.name = name;
	o.description = description;
	o.type = OptionDescription::TYPE_BOOLEAN;
	o.usage = usage;
	_optionDescriptions.push_back(o);
	if (_optionValues.find(name) == _optionValues.end())
		_optionValues[name] = defaultValue;
}

void Module::addOption(const std::string& name, const std::string& description,
		const std::vector<std::string>& defaultValue,
		OptionDescription::Usage usage)
{
	if (hasOption(name))
		return;
	OptionDescription o;
	o.name = name;
	o.description = description;
	o.type = OptionDescription::TYPE_STRING_VECTOR;
	o.usage = usage;
	_optionDescriptions.push_back(o);
	if (_optionValues.find(name) == _optionValues.end())
	{
		std::vector<Variant> &v = _optionValues[name].asVector();
		for (size_t i = 0; i < defaultValue.size(); i++)
			v.push_back(Variant::fromString(defaultValue[i]));
	}
}

const OptionDescription &Module::getOptionDescription(
		const std::string &name) const

{
	const std::vector<OptionDescription> &options = getOptionDescriptions();
	std::vector<OptionDescription>::const_iterator it;

	for (it = options.begin(); it != options.end(); it++)
	{
		if (name == it->name)
			return *it;
	}

	throw std::runtime_error(
			"Module '" + _name + "' : OptionDescription " + name
					+ " not found.");
}

void Module::getOption(const std::string &name, std::string &value) const

{
	if (hasOption(name))
		value = _optionValues.find(name)->second.toString();
	else
		logger(LOG_LEVEL_NONE, "ERROR: Module ", getName(),
				" has no option called ", name);
}

void Module::getOption(const std::string &name,
		std::vector<std::string> &value) const
{
	if (hasOption(name))
	{
		const Variant &v = _optionValues.find(name)->second;
		value.clear();
		for (size_t i = 0; i < v.asVector().size(); i++)
			value.push_back(v[i].asString());
	}
	else
		logger(LOG_LEVEL_NONE, "ERROR: Module ", getName(),
				" has no option called ", name);
}

void Module::getOption(const std::string &name, double &value) const
{
	if (hasOption(name))
		value = _optionValues.find(name)->second;
	else
		logger(LOG_LEVEL_NONE, "ERROR: Module ", getName(),
				" has no option called ", name);
}

void Module::getOption(const std::string &name, int64_t &value) const
{
	if (hasOption(name))
		value = _optionValues.find(name)->second;
	else
		logger(LOG_LEVEL_NONE, "ERROR: Module ", getName(),
				" has no option called ", name);
}

void Module::getOption(const std::string &name, bool &value) const
{
	if (hasOption(name))
		value = _optionValues.find(name)->second;
	else
		logger(LOG_LEVEL_NONE, "ERROR: Module ", getName(),
				" has no option called ", name);
}

void Module::setOption(const std::string &name, const std::string &value)
{
	if (!hasOption(name))
		throw std::runtime_error("Trying to set unknown option: " + name);
	_optionValues[name] = value;
}

void Module::setOption(const std::string &name,
		const std::vector<std::string> &value)
{
	if (!hasOption(name))
		throw std::runtime_error("Trying to set unknown option: " + name);
	std::vector<Variant> &v = _optionValues[name].asVector();
	v.clear();
	for (size_t i = 0; i < value.size(); i++)
		v.push_back(Variant::fromString(value[i]));
}

void Module::setOption(const std::string &name, const double &value)
{
	if (!hasOption(name))
		throw std::runtime_error("Trying to set unknown option: " + name);
	_optionValues[name] = value;
}

void Module::setOption(const std::string &name, const int64_t &value)
{
	if (!hasOption(name))
		throw std::runtime_error("Trying to set unknown option: " + name);
	_optionValues[name] = value;
}

void Module::setOption(const std::string &name, const bool &value)
{
	if (!hasOption(name))
		throw std::runtime_error("Trying to set unknown option: " + name);
	_optionValues[name] = value;
}

bool Module::hasOption(const std::string &name) const
{
	std::vector<OptionDescription>::const_iterator it;
	for (it = _optionDescriptions.begin(); it != _optionDescriptions.end();
			it++)
	{
		if (name.compare(it->name) == 0)
			return true;
	}
	return false;
}

bool Module::hasOption(const std::string &name, OptionDescription::Type type)
{
	std::vector<OptionDescription>::const_iterator it;
	for (it = _optionDescriptions.begin(); it != _optionDescriptions.end();
			it++)
	{
		if ((name.compare(it->name) == 0) && type == it->type)
			return true;
	}
	return false;
}

bool Module::hasSink(const std::string &name)
{
	std::vector<SinkDescription>::const_iterator it;
	for (it = _sinkDescriptions.begin(); it != _sinkDescriptions.end(); it++)
	{
		if (name == it->name)
			return true;
	}
	return false;
}

bool Module::hasSource(const std::string &name)
{
	std::vector<SourceDescription>::const_iterator it;
	for (it = _sourceDescriptions.begin(); it != _sourceDescriptions.end();
			it++)
	{
		if (name == it->name)
			return true;
	}
	return false;
}

void Module::setAnalysis(Analysis* analysis)
{
	_analysis = analysis;
}

void Module::setName(const std::string &name)
{
	if (_analysis)
	{
		if (_analysis->hasModule(name))
		{
			throw std::runtime_error("Module in analysis and name not unique");
		}
	}
	_name = name;
}

void Module::initialize()
{
	//_statusMessages.clear();
}

void Module::shutdown()
{
	_sourceDescriptions.clear();
	_sinkDescriptions.clear();
	_optionDescriptions.clear();
	_sources.clear();
	_sinks.clear();
	_optionValues.clear();
}

void Module::destroy()
{
	delete this;
}

bool Module::isRunnable() const
{
	return false;
}

void Module::clearDescriptions()
{
	_sourceDescriptions.clear();
	_sinkDescriptions.clear();
	_optionDescriptions.clear();
}

void Module::syncDescriptions()
{
	// remove sinks w/o option
	std::map<std::string, Sink>::iterator iSink;
	for (iSink = _sinks.begin(); iSink != _sinks.end(); iSink++)
	{
		if (hasSink(iSink->first) == false)
			_sinks.erase(iSink);
	}

	// remove sources w/o option
	std::map<std::string, Source>::iterator iSource;
	for (iSource = _sources.begin(); iSource != _sources.end(); iSource++)
	{
		if (hasSource(iSource->first) == false)
			_sources.erase(iSource);
	}

	// remove doubles w/o option
	std::map<std::string, Variant>::iterator iValue;
	for (iValue = _optionValues.begin(); iValue != _optionValues.end();
			iValue++)
	{
		if (hasOption(iValue->first) == false)
			_optionValues.erase(iValue);
	}
}

void Module::reload()
{
	clearDescriptions();
	initialize();
	syncDescriptions();
}

void Module::onPublish(size_t messageid, const pxl::Variant &v)
{

}

} // namespace pxl
