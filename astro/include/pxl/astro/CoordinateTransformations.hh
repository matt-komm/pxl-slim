//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#ifndef PXL_ASTRO_COORDINATETRANSFORMATIONS_HH
#define PXL_ASTRO_COORDINATETRANSFORMATIONS_HH
#include "pxl/core/macros.hh"

#include <cmath>
#include <cstdlib>
#include "pxl/core/Basic3Vector.hh"
#include "pxl/core/BasicMatrix.hh"
namespace pxl
{
/// Implements various transformations for astronomical coordinates
class PXL_DLL_EXPORT CoordinateTransformations
{
private:
	///Rotation matrices for conversion
	static double _R1950[9];
	static double _R2000[9];
	static double _SG[9];

public:
	enum Epoque_T
	{
		J1950, J2000
	};

	/// Rotates the direction given by vec from Galactic coordinates to equatorial coordinates
	static Basic3Vector convertGalactic2Equatorial(const Basic3Vector& vec,
			Epoque_T epoque = J2000);
	static Basic3Vector convertEquatorial2Galactic(const Basic3Vector& vec,
			Epoque_T epoque = J2000);

	static Basic3Vector convertSuperGalactic2Equatorial(const Basic3Vector& vec,
			Epoque_T epoque = J2000);
	static Basic3Vector convertEquatorial2SuperGalactic(const Basic3Vector& vec,
			Epoque_T epoque = J2000);

	static Basic3Vector convertGalactic2SuperGalactic(const Basic3Vector& vec);

	static Basic3Vector convertSuperGalactic2Galactic(const Basic3Vector& vec);
	static double timeAngle2Radian(unsigned int hours, unsigned int minutes,
			double seconds);
	static double minutesOfArc2Radian(int degree, int minutes, double seconds);

};
}

#endif

