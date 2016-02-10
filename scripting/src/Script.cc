//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#include "pxl/scripting/Script.hh"
#include "pxl/core/logging.hh"
#include "pxl/core/functions.hh"

#include <sstream>
#include <fstream>
#include <stdexcept>
#include <Python.h>
#include <frameobject.h>
#if (PY_VERSION_HEX < 0x02050000)
typedef int Py_ssize_t;
#endif


#if PY_MAJOR_VERSION < 3

//#define PyLong_FromLong PyLong_FromLong
//#define PyString_FromString PyString_FromString
//#define PyString_FromStringAndSize PyString_FromStringAndSize
//#define PyString_Check PyUnicode_Check

static char ARG0[] = "";
static char *SYS_ARGV[1] = {ARG0};

#else

static wchar_t ARG0[] = L"";
static wchar_t *SYS_ARGV[1] = { ARG0 };

#endif

#include "swigpyrun.h"

namespace pxl {

static Logger logger("pxl::Script");

Script::Script() :
		context(0) {
	initialize();
}

Script::~Script() {
	destroy();
}

void Script::loadModule(char *src, char *dest)
// do not change parameters to CONST CHAR *!! This is incompatible with older SWIG versions.
		{
	if (context == 0)
		return;

	PyObject *module = PyImport_ImportModule(src);
	if (module) {
		PyDict_SetItemString(context, dest, module);
		Py_DECREF(module);
	}
}

bool Script::create(const std::string& script) {
	return create(script, "embedded script");
}

bool Script::create(const std::string& script, const std::string& filename) {
	logger(LOG_LEVEL_DEBUG, "create '", filename, "'");

	if (context != 0)
		return false;

	PyObject* main_module = PyImport_AddModule("__main__");
	if (main_module == 0) {
		//logger(LOG_LEVEL_DEBUG, "Error creating script", script, "from filename", filename, fetchPythonError());
		throw std::runtime_error(
				"Script::create: error getting __main__ module");
	}
	std::string fileDirectory = getParentDirectory(filename);
	if (!fileDirectory.empty())
	{
		addModulePath(fileDirectory);
	}

	PyObject *main_dict = PyModule_GetDict(main_module);

	context = PyDict_Copy(main_dict);
	loadModule((char *) "pxl.core", (char *) "core");
	loadModule((char *) "pxl.hep", (char *) "hep");
	loadModule((char *) "pxl.astro", (char *) "astro");
	loadModule((char *) "pxl.algorithms", (char *) "algorithms");
	loadModule((char *) "pxl.modules", (char *) "modules");
	loadModule((char *) "pxl.xml", (char *) "xml");
	PyDict_SetItem(context, PyString_FromString("__file__"),
			PyString_FromString(filename.c_str()));

	logger(LOG_LEVEL_DEBUG, "Running Script");
	PyObject* runObj =
			PyRun_String(script.c_str(), Py_file_input, context, context);
	logger(LOG_LEVEL_DEBUG, "Compiling Script");
	PyObject* compiled =
			Py_CompileString(script.c_str(), filename.c_str(), Py_file_input);
	if (compiled == NULL || runObj == NULL) {
		//logger(LOG_LEVEL_ERROR, "Error creating script", script, "from filename", filename);
		logger(LOG_LEVEL_ERROR, "Error creating script from filename", filename);
		releaseObject(context);
		throw std::runtime_error(
				"Script::create: error compiling script: " + filename);
	}
	return true;
}

PyObject *Script::getContext() {
	return context;
}

bool Script::load(const std::string& filename) {
	int length;

	std::ifstream is;
	is.open(filename.c_str());

	if (is.good() == false) {
		logger(LOG_LEVEL_WARNING, "file '", filename, "'not found.");
		return false;
	}

	// get length of file:
	is.seekg(0, std::ios::end);
	length = is.tellg();
	is.seekg(0, std::ios::beg);

	// allocate memory:
	std::string buffer(length + 1, '\0');
	bool result;
	try {
		// read data as a block:
		is.read(&buffer[0], length);
		is.close();
		result = create(buffer, filename);
	} catch (std::exception &e) { // delete buffer first, then rethrow exception
		throw std::runtime_error(e.what());
	}
	return result;
}

void Script::destroy() {
	releaseObject(context);
}

PyObject *Script::parseParameters(const std::string &parameters) {
	PyObject *dict = PyDict_New();

	std::string key, value;
	bool stringFlag = false;
	bool keyFlag = true;

	for (std::string::const_iterator i = parameters.begin();
			i != parameters.end(); i++) {
		if (*i == '"') {
			stringFlag = !stringFlag;
		} else if (stringFlag) {
			if (keyFlag)
				key.append(1, *i);
			else
				value.append(1, *i);
		} else if (*i == '=') {
			keyFlag = false;
		} else if (*i == ',') {
			if (!key.empty()) {
				PyDict_SetItem(dict, PyString_FromString(trim(key).c_str()),
						PyString_FromString(trim(value).c_str()));
			}
			key.clear();
			value.clear();
			keyFlag = true;
		} else {
			if (keyFlag)
				key.append(1, *i);
			else
				value.append(1, *i);
		}
	}

	if (!key.empty()) {
		PyDict_SetItem(dict, PyString_FromString(trim(key).c_str()),
				PyString_FromString(trim(value).c_str()));
	}

	return dict;
}

PyObject *Script::createArgumentList(std::vector<PyObject *> objects) {
	PyObject *args = PyTuple_New(objects.size());
	if (args == 0)
		return 0;

	for (size_t i = 0; i < objects.size(); i++)
		PyTuple_SET_ITEM(args, i, objects[i]);

	return args;
}

PyObject *Script::getClassWithMethod(const std::string &name) {
	if (context == 0)
		return 0;

	PyObject *key, *value;
	Py_ssize_t pos = 0;

	while (PyDict_Next(context, &pos, &key, &value)) {
		if (PyObject_HasAttrString(value, const_cast<char*>(name.c_str()))) {
			return value;
		}
	}
	return 0;
}

PyObject *Script::getFunction(const char *name) {
	if (context == 0)
		return 0;

	return PyDict_GetItemString(context, name);
}

PyObject *Script::getMethod(PyObject *object, const std::string &name) {
	PyObject *method = PyObject_GetAttrString(object,
			const_cast<char *>(name.c_str()));
	if (method == NULL) {
		PyErr_Clear();
		logger(LOG_LEVEL_INFO, "getMethod:", name, "not found");
		return 0;
	}

	if (!PyCallable_Check(method)) {
		Py_XDECREF(method);
		logger(LOG_LEVEL_WARNING, "getMethod:", name, "not callable");
		return 0;
	}

	return method;
}

PyObject *Script::call(const char *name, PyObject* args) {
	PyObject *function = getFunction(name);
	if (function) {
		return call(function, args);
	} else {
		return 0;
	}
}

PyObject *Script::call(PyObject *function, std::vector<PyObject *> arguments) {
	PyObject *args = createArgumentList(arguments);
	return call(function, args);
}

PyObject *Script::call(PyObject *function, PyObject* args) {
	if (function == 0) {
		Py_XDECREF(args);
		return 0;
	}

	PyObject* resultObject = PyObject_CallObject(function, args);
	Py_XDECREF(args);
	if (resultObject) {
		return resultObject;
	} else {
		//logger(LOG_LEVEL_DEBUG, "Error executing python function", fetchPythonError());
		throw std::runtime_error(fetchPythonError());
	}
}

PyObject *Script::createObjectFromPointer(const char *name, void *ptr,
		bool own) {
	std::string fullname("_p_pxl__");
	fullname.append(name);
	swig_type_info *type_info = SWIG_TypeQuery(fullname.c_str());
	if (type_info == 0)
		return 0;

	return SWIG_NewPointerObj(ptr, type_info, own ? SWIG_POINTER_OWN : 0);
}

PyObject *Script::createObjectFromLong(long i) {
	return PyLong_FromLong(i);
}

PyObject *Script::createObjectFromString(const std::string &str) {
#if PY_MAJOR_VERSION < 3
	return PyString_FromStringAndSize(str.c_str(), str.size());
#else
	return PyUnicode_FromStringAndSize(str.c_str(), str.size());
#endif
}

void *Script::createPointerFromObject(const char *name, PyObject *obj,
		bool own) {
	std::string fullname("_p_pxl__");
	fullname.append(name);
	swig_type_info *type_info = SWIG_TypeQuery(fullname.c_str());
	if (type_info == 0)
		return 0;

	void *ptr;
	int doown;
	SWIG_ConvertPtrAndOwn(obj, &ptr, type_info, own ? SWIG_POINTER_DISOWN : 0,
			&doown);
	return ptr;
}

void Script::releaseObject(PyObject *&ptr) {
	Py_XDECREF(ptr);
	ptr = 0;
}

double Script::objectToDouble(PyObject *obj) {
	return PyFloat_AsDouble(obj);
}

long Script::objectToLong(PyObject *obj) {
	return PyLong_AsLong(obj);
}

std::string Script::objectToString(PyObject *obj) {
	std::string s;
	char* ascii = NULL;
	#if PYTHON_VERSION < 27
		ascii = PyString_AsString(obj);
	#else
		ascii = PyBytes_AsString(obj);
	#endif
	if (ascii)
	{
		s = ascii;
	}
	return s;
}

bool Script::objectIsString(PyObject *obj) {
	#if PYTHON_VERSION < 27
		return PyString_Check(obj);
	#else
		return PyBytes_Check(obj);
	#endif
}

bool Script::objectToBool(PyObject *obj)
{
	return Py_True == obj;
}

Serializable *Script::objectToSerializable(PyObject *obj, bool own)
{
	if (obj == Py_None)
		return 0;

	return (Serializable *) Script::createPointerFromObject("Serializable", obj, own);
}

void Script::initialize()
{
	if (Py_IsInitialized() == 0)
	{
		Py_Initialize();
		PySys_SetArgv(1, SYS_ARGV);
		// Remove "" from system path to avoid security issue
		// http://docs.python.org/c-api/init.html#PySys_SetArgvEx
		// http://cve.mitre.org/cgi-bin/cvename.cgi?name=CVE-2008-5983
		PyRun_SimpleString("import sys; sys.path.pop(0)\n");
	}
}


void Script::addModulePath(const std::string &path)
{
	std::string script = "import sys; sys.path.append(\"" + path + "\")\n";
	PyRun_SimpleString(script.c_str());
}


std::string Script::fetchPythonError(const std::string &from)
{
	std::stringstream returnValue;
	PyObject *ptype = NULL;
	PyObject *pvalue = NULL;
	PyObject *pstring = NULL;
	PyObject *pTmpTraceback= NULL;

	PyErr_Fetch(&ptype, &pvalue, &pTmpTraceback);
	PyErr_NormalizeException(&ptype, &pvalue, &pTmpTraceback);
  PyTracebackObject *ptraceback = (PyTracebackObject *)pTmpTraceback;
	if (ptraceback!= NULL)
  {
    returnValue << "Got Python traceback:\n";
    returnValue << "\tfrom \x22" << from << "\x22, line ";
    returnValue << ptraceback->tb_lineno;
    returnValue << ", in ";
    returnValue << PyString_AsString(ptraceback->tb_frame->f_code->co_name);
    returnValue << "\n";
    ptraceback = ptraceback->tb_next;

    int err = 0;
    while (ptraceback != NULL && err == 0)
    {
      returnValue << "\tFile \x22";
      returnValue << PyString_AsString(ptraceback->tb_frame->f_code->co_filename);
      returnValue << "\x22, line ";
      returnValue << ptraceback->tb_lineno;
      returnValue << ", in ";
      returnValue << PyString_AsString(ptraceback->tb_frame->f_code->co_name);
      returnValue << "\n";

      ptraceback = ptraceback->tb_next;
      if (err == 0)
        err = PyErr_CheckSignals();
    }
  }
  if (ptype!= NULL)
  {
    PyTypeObject *pyt = (PyTypeObject *)ptype;
    returnValue << "\t" << pyt->tp_name;
//      << PyString_AsString(PyString_AsString(ptype)) <<"\n";
    if (pvalue != NULL)
	  {
		  pstring = PyObject_Str(pvalue);
		  if (pstring != NULL)
		  {
			  const char *chr = PyString_AsString(pstring);
			  if (chr != 0)
			  {
				  returnValue << "  " << chr << "\n";
			  }
		  }
		  Py_XDECREF(pstring);
    }
    else
    {
      returnValue << "\n";
    }
	}


	Py_XDECREF(ptype);
	Py_XDECREF(pvalue);
	Py_XDECREF(ptraceback);

	return returnValue.str();
}

} // namespace pxl
