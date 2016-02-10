//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#include "pxl/astro/AstroBasicObject.hh"
#include <cmath>
namespace pxl
{


	void AstroBasicObject::serialize(const OutputStream &out) const
	{
		Serializable::serialize(out);
		_vector.serialize(out);
		out.writeDouble(_dAngularResolution);
		out.writeDouble(_dDummy);
		out.writeLong((long)_time);
	}

	void AstroBasicObject::deserialize(const InputStream &in)
	{
		Serializable::deserialize(in);
		_vector.deserialize(in);
		in.readDouble(_dAngularResolution);
		in.readDouble(_dDummy);
		long t;
		in.readLong(t);
		_time = t;
	}

	std::ostream& AstroBasicObject::print(int level, std::ostream& os, int pan) const
	{
		os << " Galactic Latitude : " << getGalacticLatitude() << std::endl;
		os << "Galactic Longitude : " << getGalacticLongitude() << std::endl;
		return os;
	}


	void AstroBasicObject::setGalacticCoordinates(double longitude,double latitude)
	{
		_vector.setX(cos(longitude) * cos(latitude));
		_vector.setY(sin(longitude) * cos(latitude));
		_vector.setZ(sin(latitude));
		// normalize vector!
		_vector/=sqrt(_vector*_vector);
	}


	void AstroBasicObject::setEquatorialCoordinates(
			unsigned int hours, unsigned int minutes, double seconds,
			int degree, int moa, double soa,
			CoordinateTransformations::Epoque_T epoque)
	{
		setEquatorialCoordinates(
				CoordinateTransformations::timeAngle2Radian(hours, minutes, seconds),
				CoordinateTransformations::minutesOfArc2Radian(degree,moa,soa),
				epoque);
	}

	void AstroBasicObject::setEquatorialCoordinates(double rightascension, double declination, CoordinateTransformations::Epoque_T epoque)
	{
		_vector.setX(cos(rightascension) * cos(declination));
		_vector.setY(sin(rightascension) * cos(declination));
		_vector.setZ(sin(declination));
		_vector = CoordinateTransformations::convertEquatorial2Galactic(_vector);
		_vector/=sqrt(_vector*_vector);
	}

	double AstroBasicObject::getRightAscension(CoordinateTransformations::Epoque_T epoque) const
	{
		Basic3Vector t = CoordinateTransformations::convertGalactic2Equatorial(_vector, epoque);
		double ra = atan2(t.getY(),t.getX());
		// RightAscension si between 0 and 2*pi
		if (ra < 0)
			ra+=2*M_PI;
		return ra;

	}

void AstroBasicObject::setSuperGalacticCoordinates(double longitude,double latitude)
	{
		_vector.setX(cos(longitude) * cos(latitude));
		_vector.setY(sin(longitude) * cos(latitude));
		_vector.setZ(sin(latitude));
		_vector = CoordinateTransformations::convertSuperGalactic2Galactic(_vector);
		_vector/=sqrt(_vector*_vector);
	}

	double AstroBasicObject::getDeclination(CoordinateTransformations::Epoque_T epoque) const
	{
		Basic3Vector t = CoordinateTransformations::convertGalactic2Equatorial(_vector, epoque);
		return asin(t.getZ()/sqrt(t*t));
	}

	double AstroBasicObject::getSuperGalacticLongitude() const
	{
		Basic3Vector t = CoordinateTransformations::convertGalactic2SuperGalactic(_vector);
		return atan2(t.getY(),t.getX());
	}

	double AstroBasicObject::getSuperGalacticLatitude() const
	{
		Basic3Vector t = CoordinateTransformations::convertGalactic2SuperGalactic(_vector);
		return asin(t.getZ()/sqrt(t*t));
	}

	double AstroBasicObject::getGalacticLongitude() const
	{
		return atan2(_vector.getY(),_vector.getX());
	}

	double AstroBasicObject::getGalacticLatitude() const
	{
		return asin(_vector.getZ());
	}

	double AstroBasicObject::angularDistanceTo(const AstroBasicObject &obj) const
	{
		return angularDistanceTo(obj._vector);
	}

	double AstroBasicObject::angularDistanceTo(const Basic3Vector &obj) const
	{
		return _vector.getAngleTo(obj);
	}

	double AstroBasicObject::angularDistanceTo(const AstroBasicObject *obj) const
	{
		return angularDistanceTo(obj->_vector);
	}

}
