//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//          http://pxl.sourceforge.net      -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Contact: pxl-users@lists.sourceforge.net -
//-------------------------------------------

#include "pxl/core.hh"
#include "pxl/core/macros.hh"
#include "pxl/core/PluginManager.hh"
#include "pxl/core/Serializable.hh"
#include "pxl/core/Id.hh"

#include <iostream>


class ExampleUserObject : public pxl::Serializable
{
private:

public:
/// Returns the Type Id of the Serializable derivative

  
  virtual pxl::Serializable* clone() const
  {
  	return new ExampleUserObject(*this);
  }

	virtual const pxl::Id& getTypeId() const
	{
		return getStaticTypeId();
	}

	/// Returns the Type Id of the Serializable derivative
	static const pxl::Id& getStaticTypeId()
	{
		static const pxl::Id id("074fd3f9-fbf3-418e-9b33-1459bbd65e77");
		return id;
	}

	void testMethod();

};


