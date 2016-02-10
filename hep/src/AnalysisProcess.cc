//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#include <iostream>
#include <string>

#include "pxl/hep/AnalysisProcess.hh"

namespace pxl
{

Serializable* AnalysisProcess::clone() const
{
	return new AnalysisProcess(*this);
}

std::ostream& AnalysisProcess::print(int level, std::ostream& os, int pan) const
{
	printPan1st(os, pan) << "AnalysisProcess: " << getName() << std::endl;

	if (level>0) os << printContent(level, os, pan);

	for (ObjectOwner::const_iterator iter = getObjectOwner().begin(); iter!=getObjectOwner().end(); ++iter)
	{
		if ((*iter)->getMotherRelations().size() == 0)
			(*iter)->printDecayTree(level, os, pan);
	}
	return os;
}



} // namespace pxl
