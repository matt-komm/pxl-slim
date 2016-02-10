 //-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#ifndef PXL_CORE_CONFIGLOADER
#define PXL_CORE_CONFIGLOADER


#include "pxl/core/macros.hh"

#include "simpleini/SimpleIni.h"

namespace pxl
{

/// Add all items from a CSimpleIniA object to the pcl Configuration instance
void PXL_DLL_EXPORT fillConfigFromSimpleIni(const CSimpleIniA &iniFile);
/// loads configurations from /etc/pxlrc and HOME/.pxlrc
void PXL_DLL_EXPORT loadDefaultConfigurations();


} // namespace

#endif //PXL_CORE_CONFIGLOADER
