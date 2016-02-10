//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#include "pxl/astro/UHECR.hh"

namespace pxl
{

std::ostream& UHECR::print(int level, std::ostream& os, int pan) const
{
	os << " Latitude : " << getGalacticLatitude() << std::endl;
	os << "Longitude : " << getGalacticLongitude() << std::endl;
	os << "   Energy : " << getEnergy() << std::endl;
	return os;
}

void UHECR::serialize(const OutputStream &out) const
{
	AstroObject::serialize(out);
	out.writeDouble(_energy);
	out.writeDouble(_denergy);
	out.writeDouble(_mass);
	out.writeDouble(_charge);
}

void UHECR::deserialize(const InputStream &in)
{
	AstroObject::deserialize(in);
	in.readDouble(_energy);
	in.readDouble(_denergy);
	in.readDouble(_mass);
	in.readDouble(_charge);
}

}
