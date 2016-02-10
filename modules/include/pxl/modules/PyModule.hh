//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#ifndef PXL_PY_MODULE_HH
#define PXL_PY_MODULE_HH
#include "pxl/core/macros.hh"

#include "pxl/scripting/Script.hh"
#include "pxl/modules/Module.hh"

namespace pxl
{

class PXL_DLL_EXPORT PyModule: public Module
{
protected:
	Script _Script;
	PyObject *_Instance;
	PyObject *_AnalyseMethod;
	PyObject *_BeginRunMethod;
	PyObject *_EndRunMethod;
	PyObject *_BeginJobMethod;
	PyObject *_EndJobMethod;
	PyObject *_OnPublishMethod;
	std::string _AnalyseMethodName;
	std::string _filename;

public:
	PyModule();
	~PyModule();
	Script &getScript();
	PyObject *getInstance();
	PyObject *getAnalyseMethod();

	void initialize();

	void setAnalyseMethodName(const std::string &name);

	void loadDefaultMethods(PyObject *object);

	void shutdown();

	void beginRun();

	void endRun();

	void beginJob();

	void beginJob(const std::string &parameters);

	void endJob();

	void setOption(const std::string &name, const std::string &value);

	void reload();
};

} // namespace pxl

#endif
