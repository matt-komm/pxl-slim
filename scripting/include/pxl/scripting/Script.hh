//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#ifndef PXL_SCRIPT_HH
#define PXL_SCRIPT_HH

#include <string>
#include <vector>
#include "pxl/core/macros.hh"
#include "pxl/core/Serializable.hh"

typedef struct _object PyObject;

namespace pxl {
/**
 base class for python scripts
 */

class PXL_DLL_EXPORT Script
{
public:
	Script();
	virtual ~Script();

	bool create (const std::string& script);
	virtual bool create (const std::string& script, const std::string& filename);
	virtual bool load (const std::string& filename);
	virtual void destroy();
  
	void loadModule(char *src, char *dest);
	// do not change parameters of loadModule to CONST CHAR *!! This is incompatible with older SWIG versions.

	PyObject *getContext();
	PyObject *call(const char *name, PyObject* arguments);
	PyObject *getFunction(const char *name);
	PyObject *getClassWithMethod(const std::string &name);

	static PyObject *call(PyObject *function, std::vector<PyObject *> arguments);
	static PyObject *call(PyObject *function, PyObject* arguments);
	static PyObject *getMethod(PyObject *object, const std::string &name);
	static PyObject *parseParameters(const std::string &parameters);
	static PyObject *createArgumentList(std::vector<PyObject *> objects);
	static PyObject *createObjectFromPointer (const char *name, void *ptr, bool own);
	static PyObject *createObjectFromLong (long i);
	static PyObject *createObjectFromString (const std::string &);
	static void *createPointerFromObject (const char *name, PyObject *obj, bool own);
	static void releaseObject(PyObject *&ptr);
	static double objectToDouble(PyObject *obj);
	static long objectToLong(PyObject *obj);
	static std::string objectToString(PyObject *obj);
	static bool objectIsString(PyObject *obj);
	static bool objectToBool(PyObject *obj);
	static Serializable *objectToSerializable(PyObject *obj, bool own);
	static void initialize();
	static void addModulePath(const std::string &path);
	static std::string fetchPythonError(const std::string &from = "unknown context");

private:
	PyObject *context;
};

} // namespace pxl

#endif // PXL_SCRIPT_FILTER_HH

