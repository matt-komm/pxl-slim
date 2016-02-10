//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#ifndef PXL_ASTRO_SOURCE_HH
#define PXL_ASTRO_SOURCE_HH
#include "pxl/core/macros.hh"

#include "pxl/astro/AstroObject.hh"
#include <iostream>
#include <string>

namespace pxl
{
/**
 UHECRSource are objects in the sky, which can be
 named and related to other AstroObjects with SoftRelations,but have a distance and a  luminosity
 */

class PXL_DLL_EXPORT UHECRSource: public AstroObject
{
public:

	UHECRSource() : AstroObject(), _luminosity(1.0), _distance(1.0), _name("Source")
	{
	}

	UHECRSource(const UHECRSource& original) :
	AstroObject(original), _luminosity(original._luminosity), _distance(original._distance),
		_name(original._name)
	{
	}

	explicit UHECRSource(const UHECRSource* original) :
	AstroObject(original),_luminosity(original->_luminosity), _distance(original->_distance),
		_name(original->_name)
	{
	}

	UHECRSource(double longitude, double latitude, double distance=1., double luminosity=1.) : AstroObject(longitude, latitude), _luminosity(luminosity), _distance(distance), _name("Source")
	{
	}

	UHECRSource(const Basic3Vector& vector, double distance=1., double luminosity=1.) : AstroObject(vector), _luminosity(luminosity), _distance(distance), _name("Source")
	{
	}

	virtual ~UHECRSource()
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
		static const Id id("0855b033-8b1a-c7e0-c816e0044b041bff");
		return id;
	}

	/// Creates a deep copy and returns a C++ pointer to the newly-created object.
	virtual Serializable* clone() const
	{
		return new UHECRSource(*this);
	}

	/// Prints out some information about the CR
	virtual std::ostream& print(int level=1, std::ostream& os=std::cout, int pan=1) const;

	inline double getLuminosity() const
	{
		return _luminosity;
	}

	inline double getDistance() const
	{
		return _distance;
	}

	inline std::string getName() const
	{
		return _name;
	}

	inline void setDistance(double distance)
	{
		_distance = distance;
	}

	inline void setLuminosity(double luminosity)
	{
		_luminosity = luminosity;
	}

	void setName(std::string name)
	{
		_name = name;
	}

	void setGalacticXYZ(double x, double y, double z)
	{
		AstroBasicObject::setGalacticXYZ(x,y,z);
		//Set Distance
		_distance = sqrt(x*x + y*y + z*z);
	}

	void setSuperGalacticXYZ(double x, double y, double z)
	{
		AstroBasicObject::setSuperGalacticXYZ(x,y,z);
		//Set Distance
		_distance = sqrt(x*x + y*y + z*z);
	}

	void setEquatorialXYZ(double x, double y, double z)
	{
		AstroBasicObject::setEquatorialXYZ(x,y,z);
		//Set Distance
		_distance = sqrt(x*x + y*y + z*z);
	}


private:
	double _luminosity;
	double _distance;
	std::string _name;

	UHECRSource& operator=(const UHECRSource& original)
	{
		return *this;
	}
};
}
//namespace pxl

#endif
