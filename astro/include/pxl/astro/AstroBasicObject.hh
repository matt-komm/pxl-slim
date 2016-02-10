//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#ifndef PXL_ASTRO_ASTRO_BASIC_OBJECT_HH
#define PXL_ASTRO_ASTRO_BASIC_OBJECT_HH
#include "pxl/core/macros.hh"

#include "pxl/core/Basic3Vector.hh"
#include "pxl/core/Serializable.hh"
#include "pxl/core/ObjectFactory.hh"
#include "pxl/core/Id.hh"
#include "pxl/astro/CoordinateTransformations.hh"

namespace pxl
{
class BasicContainer;

/**
 This Class contains basic astrophysical objects, observed at a
 direction at a given time. The direction is stored as a vector with 3
 comonents in galactic coordinates, using longitude (-pi..pi) and
 latitude (-pi/2 .. pi/2). Equatorial Coordinates are calculated on the
 fly and returned as right ascension (-pi..pi) and declination  (-pi/2
 .. pi/2).
 */
class PXL_DLL_EXPORT AstroBasicObject : public Serializable
{
public:
	AstroBasicObject() : Serializable(), _time(0),_vector(1,0,0),_dAngularResolution(0),_dDummy(0)
	{
	}

	AstroBasicObject(double longitude, double latitude) : Serializable(), _time(0),_vector(1,0,0),_dAngularResolution(0),_dDummy(0)
	{
		setGalacticCoordinates(longitude,latitude);
	}

	AstroBasicObject(const Basic3Vector& vector) : Serializable(), _time(0),_vector(vector),_dAngularResolution(0),_dDummy(0)
	{
	}


	AstroBasicObject(const AstroBasicObject& original) :
	Serializable(), _time(original._time),
	_vector(original._vector),_dAngularResolution(original._dAngularResolution), _dDummy(original._dDummy)
	{
	}

	explicit AstroBasicObject(const AstroBasicObject* original) :
	Serializable(), _time(original->_time),
	_vector(original->_vector),_dAngularResolution(original->_dAngularResolution), _dDummy(original->_dDummy)
	{
	}

	virtual ~AstroBasicObject()
	{
	}


	/// Prints out some information about the AstroBasicObject 
	virtual std::ostream& print(int level=1, std::ostream& os=std::cout, int pan=1) const;

	/// Set position in galactic corrdinates 
	void setGalacticCoordinates(double longitude,double latitude);

	void setSuperGalacticCoordinates(double longitude,double latitude);

	/// Set position in equatorial coordinates 
	void setEquatorialCoordinates(double ra,double d, CoordinateTransformations::Epoque_T epoque=CoordinateTransformations::J2000);

	void setEquatorialCoordinates( unsigned int hours, unsigned int minutes, double seconds,
			int degree, int moa, double soa, CoordinateTransformations::Epoque_T epoque);

	/// Sets an error for the angular resolution 
	inline void setAngularResolution(double value)
	{
		_dAngularResolution = value;
	}

	/// returns the error on the longitude 
	inline double getAngularResolution()
	{
		return _dAngularResolution;
	}

	///Sets a time asoziated with the observation in Unix time format
	inline void setTime(const time_t &time)
	{
		_time = time;
	}

	///Returns the Declination 
	double getRightAscension(CoordinateTransformations::Epoque_T epoque=CoordinateTransformations::J2000) const;

	///Returns the Declination 
	double getDeclination(CoordinateTransformations::Epoque_T epoque=CoordinateTransformations::J2000) const;

	///Returns the longitude in super galactic coordinates
	double getSuperGalacticLongitude() const;

	///Returns the latitude in super galactic coordinates
	double getSuperGalacticLatitude() const;

	///Returns the longitude in galactic coordinates
	double getGalacticLongitude() const;

	///Returns the latitude in galactic coordinates
	double getGalacticLatitude() const;

	///Sets the components of the direction of the object
	void setSuperGalacticXYZ(double x, double y, double z)
	{
		Basic3Vector v(x,y,z);
		v.normalize();
		setSuperGalacticDirectionVector(v);
	}

	///Sets the components of the direction of the object
	void setEquatorialXYZ(double x, double y, double z)
	{
		Basic3Vector v(x,y,z);
		v.normalize();
		setEquatorialDirectionVector(v);
	}

	///Sets the components of the direction of the object
	void setGalacticXYZ(double x, double y, double z)
	{
		_vector.setXYZ(x,y,z);
		//normalize vector
		_vector.normalize();
	}

	///Sets the direction vector of the object in Galactic Coordinates
	void setGalacticDirectionVector(const Basic3Vector& vector)
	{
		_vector = vector;
		_vector/=sqrt(_vector*_vector);
	}

	///Returns the 3component Vector of the direction of the object in
	///galactic cordinates
	inline const Basic3Vector& getGalacticDirectionVector() const
	{
		return _vector;
	}

	///Sets the direction vector of the object in super galactic coordinates
	inline void setSuperGalacticDirectionVector(const Basic3Vector& vector)
	{
		_vector = vector;
		_vector = CoordinateTransformations::convertSuperGalactic2Galactic(_vector);
		_vector/=sqrt(_vector*_vector);
	}

	///Returns the 3component Vector of the direction of the object in
	///super galactic coordinates
	inline Basic3Vector getSuperGalacticDirectionVector() const
	{
		Basic3Vector _result = CoordinateTransformations::convertGalactic2SuperGalactic(_vector);
		return _result;
	}


	///Sets the direction vector of the object in equatorial coordinates
	inline void setEquatorialDirectionVector(const Basic3Vector& vector)
	{
		_vector = vector;
		_vector = CoordinateTransformations::convertEquatorial2Galactic(_vector);
		_vector/=sqrt(_vector*_vector);
	}

	///Returns the 3component Vector of the direction of the object in
	///equatorial coordinates
	inline Basic3Vector getEquatorialDirectionVector() const
	{
		Basic3Vector _result = CoordinateTransformations::convertGalactic2Equatorial(_vector);
		return _result;
	}


	///Returns a UNIX time assoziated witnh the object
	inline time_t getTime() const
	{
		return _time;
	}

	/// Returns the angular distance to given object
	double angularDistanceTo(const AstroBasicObject &obj) const;
	///
	double angularDistanceTo(const Basic3Vector &obj) const;
	/// Returns the angular distance to given object
	double angularDistanceTo(const AstroBasicObject *obj) const;

	/// Returns the Type Id of the Serializable derivative
	virtual const Id& getTypeId() const
	{
		return getStaticTypeId();
	}

	/// Returns the Type Id of the Serializable derivative
	static const Id& getStaticTypeId()
	{
		static const Id id("033dfe86-1a17-767a-8dd9-2877496dab5f");
		return id;
	}

	/// Serializes the Object 
	virtual void serialize(const OutputStream &out) const;

	/// Deserializes the Object 
	virtual void deserialize(const InputStream &in);

	/// Creates a deep copy and returns a C++ pointer to the newly-created object.
	virtual Serializable* clone() const
	{
		return new AstroBasicObject(*this);
	}

private:
	time_t _time;           /// A UNIX time asoziated with the obhject
	Basic3Vector _vector;   /// The direction of the Object
	double _dAngularResolution;           /// Angluar error 
	double _dDummy;         /// Currently not used double, not removed due to compatibility
};

}

#endif	 // ----- #ifndef ASTROBASICOBJECT_INC	-----
