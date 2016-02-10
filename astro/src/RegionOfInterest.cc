//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#include "pxl/astro/RegionOfInterest.hh"

namespace pxl
{

std::ostream& RegionOfInterest::print(int level, std::ostream& os, int pan) const
{
	os << "Name: " << getName() << std::endl;
	os << "  Latitude : " << getGalacticLatitude() << std::endl;
	os << "  Longitude : " << getGalacticLongitude() << std::endl;
	os << "  ConeRadius: " << getConeRadius() << std::endl;
	return os;
}

void RegionOfInterest::serialize(const OutputStream &out) const
{
	AstroObject::serialize(out);
	out.writeDouble(_coneRadius);
	out.writeString(_name);
}

void RegionOfInterest::deserialize(const InputStream &in)
{
	AstroObject::deserialize(in);
	in.readDouble(_coneRadius);
	in.readString(_name);
}

bool RegionOfInterest::objectInCone(const AstroBasicObject& obj) const
{
	return objectInCone(&obj);
}

bool RegionOfInterest::objectInCone(const AstroBasicObject* obj) const
{
	return (this->angularDistanceTo(obj) < _coneRadius);
}

bool RegionOfInterest::linkIfObjectInCone(AstroObject& obj, std::string name)
{
	return linkIfObjectInCone(&obj, name);
}

bool RegionOfInterest::linkIfObjectInCone(AstroObject* obj, std::string name)
{
	if (this->angularDistanceTo(obj) < _coneRadius)
	{
		this->linkSoft(obj, name);
		return true;
	}
	return false;
}
}
