//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#ifndef PXL_ASTRO_REGION_OF_INTEREST_HH
#define PXL_ASTRO_REGION_OF_INTEREST_HH
#include "pxl/core/macros.hh"

#include "pxl/astro/AstroObject.hh"
#include <iostream>
#include <string>

namespace pxl
{
/**
 RegionOfInterest are interesting regions in the sky, which can be
 named and related to other AstroObjects with SoftRelations. The region
 is defined by a cone center, the position of the RegionOfInterest, and
 an angular distance,i.e. the "radius" of the cone.
 */

class PXL_DLL_EXPORT RegionOfInterest: public AstroObject
{
public:

	RegionOfInterest() : AstroObject(), _coneRadius(0.0), _name("ROI")
	{
	}

	RegionOfInterest(const RegionOfInterest& original) :
	AstroObject(original), _coneRadius(original._coneRadius), _name(original._name)
	{
	}

	explicit RegionOfInterest(const RegionOfInterest* original) :
	AstroObject(original), _coneRadius(original->_coneRadius), _name(original->_name)
	{
	}

	RegionOfInterest(const AstroBasicObject& original,double coneRadius=0.0, std::string name="ROI") :
	AstroObject(original), _coneRadius(coneRadius), _name(name)
	{
	}

	explicit RegionOfInterest(const AstroBasicObject* original, double coneRadius=0.0, const std::string &name = "ROI") :
	AstroObject(original), _coneRadius(coneRadius), _name(name)
	{
	}

	RegionOfInterest(double longitude, double latitude, double coneRadius=0.0, const std::string &name= "ROI") : AstroObject(longitude, latitude), _coneRadius(coneRadius), _name(name)
	{
	}

	RegionOfInterest(const Basic3Vector& vector, double coneRadius=0.0, const std::string &name= "ROI") : AstroObject(vector), _coneRadius(coneRadius), _name(name)
	{
	}

	virtual ~RegionOfInterest()
	{
	}

	virtual void serialize(const OutputStream &out) const;

	virtual void deserialize(const InputStream &in);

	virtual const Id& getTypeId() const
	{
		return getStaticTypeId();
	}

	static const Id& getStaticTypeId()
	{
		static const Id id("201e9d8b-561a-c349-bb65997c49e757a6");
		return id;
	}

	/// Creates a deep copy and returns a C++ pointer to the newly-created object.
	virtual Serializable* clone() const
	{
		return new RegionOfInterest(*this);
	}

	/// Prints out some information about the Region Of Interst 
	virtual std::ostream& print(int level=1, std::ostream& os=std::cout, int pan=1) const;

	///Returns the size of the RegionOfInterest
	inline double getConeRadius() const
	{
		return _coneRadius;
	}

	///Returns the name of the region of interest
	inline std::string getName() const
	{
		return _name;
	}

	/// Sets the size of the region of interest
	inline void setConeRadius(double coneRadius)
	{
		_coneRadius= coneRadius;
	}

	///Names the region of interest
	inline void setName(std::string name)
	{
		_name = name;
	}

	/// Return value is true, if the object is within the cone radius
	bool objectInCone(const AstroBasicObject& obj) const;
	/// Return value is true, if the object is nearer than the cone radius
	bool objectInCone(const AstroBasicObject* obj) const;

	/// Creates a soft link between the RegionOfInterest, if the object is
	/// nearer than the cone size of the RegionOfInterest
	bool linkIfObjectInCone(AstroObject& obj,std::string name="") ;
	/// Creates a soft link between the RegionOfInterest, if the object is
	/// nearer than the cone size of the RegionOfInterest
	bool linkIfObjectInCone(AstroObject* obj,std::string name="") ;

private:
	double _coneRadius; /// The Cone Size of the ROI
	std::string _name;  /// The name of the ROI 

	RegionOfInterest& operator=(const RegionOfInterest& original)
	{
		return *this;
	}
};
}
//namespace pxl

#endif
