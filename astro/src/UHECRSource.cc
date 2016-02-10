//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#include "pxl/astro/UHECRSource.hh"

namespace pxl
{

std::ostream& UHECRSource::print(int level, std::ostream& os, int pan) const
{
	os << "Name: " << getName() << std::endl;
	os << "  Latitude  : " << getGalacticLatitude() << std::endl;
	os << "  Longitude : " << getGalacticLongitude() << std::endl;
	os << "  Distance  : " << getDistance() << std::endl;
	os << "  Rel. Lum. : " << getLuminosity() << std::endl;
	return os;
}

void UHECRSource::serialize(const OutputStream &out) const
{
	AstroObject::serialize(out);
	out.writeDouble(_distance);
	out.writeDouble(_luminosity);
	out.writeString(_name);
}

void UHECRSource::deserialize(const InputStream &in)
{
	AstroObject::deserialize(in);
	in.readDouble(_distance);
	in.readDouble(_luminosity);
	in.readString(_name);
}

}
