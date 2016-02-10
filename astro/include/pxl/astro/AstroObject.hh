//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#ifndef PXL_ASTRO_ASTRO_OBJECT_HH
#define PXL_ASTRO_ASTRO_OBJECT_HH
#include "pxl/core/macros.hh"

#include "pxl/core/UserRecord.hh"
#include "pxl/core/SoftRelations.hh"
#include "pxl/astro/AstroBasicObject.hh"

namespace pxl
{
/**
 AstroBasicObject with UserRecord and SoftRelations.
 */

class PXL_DLL_EXPORT AstroObject : public AstroBasicObject, public UserRecordHelper
{
public:
	AstroObject() : AstroBasicObject()
	{
	}

	/// Copy constructor. SoftRelations are not copied since they only exist
	/// between individual objects
	AstroObject(const AstroObject& original) :
	AstroBasicObject(original), UserRecordHelper(original)
	{
	}

	/// Copy constructor. SoftRelations are not copied since they only exist
	/// between individual objects
	explicit AstroObject(const AstroObject* original) :
	AstroBasicObject(original), UserRecordHelper(*original)
	{

	}

	AstroObject(const AstroBasicObject& original) :
	AstroBasicObject(original)
	{
	}

	explicit AstroObject(const AstroBasicObject* original) :
	AstroBasicObject(original)
	{
	}

	AstroObject(double longitude, double latitude) : AstroBasicObject(longitude, latitude)
	{
	}

	AstroObject(const Basic3Vector& vector) : AstroBasicObject(vector)
	{
	}


	virtual ~AstroObject()
	{
	}

	virtual void serialize(const OutputStream &out) const;

	virtual void deserialize(const InputStream &in);

	///returns the type id of the AstroObject
	virtual const Id& getTypeId() const
	{
		return getStaticTypeId();
	}

	///returns the type id of the AstroObject
	static const Id& getStaticTypeId()
	{
		static const Id id("115a29d6-ecfe-37bc-b647-937d49705165");
		return id;
	}

	/// Creates a deep copy and returns a C++ pointer to the newly-created object.
	virtual Serializable* clone() const
	{
		return new AstroObject(*this);
	}

	/// Create a bi-directional softlink between two astroobjects
	void linkSoft(AstroObject* astroobject, const std::string& type);
	/// Create a bi-directional softlink between two astroobjects
	void linkSoft(AstroObject& astroobject, const std::string& type);

	///Removes the Softlink from the object 
	void unlinkSoft(AstroObject* astroobject, const std::string& type);
	///Removes the Softlink from the object 
	void unlinkSoft(AstroObject& astroobject, const std::string& type);

	///Returns the Soft relations of the object
	const SoftRelations& getSoftRelations() const
	{
		return _softRelations;
	}

	///Returns the Soft relations of the object
	SoftRelations& getSoftRelations()
	{
		return _softRelations;
	}


private:
	SoftRelations _softRelations;

	// Make AstroObject private since it's not clear how copied
	// SoftRelations should be managed
	AstroObject& operator=(const AstroObject& original)
	{
		return *this;
	}
};

}
#endif
