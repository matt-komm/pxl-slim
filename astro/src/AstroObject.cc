//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#include "pxl/astro/AstroObject.hh"

namespace pxl
{


void AstroObject::serialize(const OutputStream &out) const
{
	AstroBasicObject::serialize(out);
	UserRecordHelper::serialize(out);
	_softRelations.serialize(out);
}

void AstroObject::deserialize(const InputStream &in)
{
	AstroBasicObject::deserialize(in);
	UserRecordHelper::deserialize(in);
	_softRelations.deserialize(in);
}

void AstroObject::linkSoft(AstroObject* astroobject, const std::string& type)
{
	if (!_softRelations.has(astroobject, type))
	{
		_softRelations.set(astroobject, type);
		astroobject->linkSoft(this, type);
	}
}

void AstroObject::unlinkSoft(AstroObject* astroobject, const std::string& type)
{
	_softRelations.remove(astroobject, type);
	astroobject->_softRelations.remove(this, type);
}

void AstroObject::linkSoft(AstroObject &astroobject, const std::string& type)
{
	linkSoft(&astroobject, type);
}

void AstroObject::unlinkSoft(AstroObject &astroobject, const std::string& type)
{
	unlinkSoft(&astroobject, type);
}




}
