//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2010 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#include "pxl/astro/CoordinateTransformations.hh"
namespace pxl
{

// Rotation matrices for conversion x_galactic = R * x_equatorial,
// with R in Epoque 1950 or 2000, according to:
// Murray, C.A., 1989. The transformation of coordinates between
// systems of B1950.0 and J2000.0 and the principal galactic axes
// referred to J2000.0, Astron. Astrophys, 218, p.325-329
// http://adsabs.harvard.edu/abs/1989A&A...218..325M
double CoordinateTransformations::_R1950[] =
{ -0.066988739, -0.872755766, -0.483538915, +0.492728466, -0.450346958,
		0.744584633, -0.867600811, -0.188374602, 0.460199785 };
double CoordinateTransformations::_R2000[] =
{ -0.054875539, -0.873437105, -0.483834992, +0.494109454, -0.444829594,
		0.746982249, -0.867666136, -0.198076390, 0.455983795 };

// Rotation matrix for conversion x_supergalactic = R * x_galactic,
// is calculated as described in the appendix of
// John L. Tonry et. al., 2000,  The Surface Brightness Fluctuation Survey of Galaxy Distances. II. Local and Large-Scale Flows, doi: 10.1086/308409, The Astrophysical Journal
// http://iopscience.iop.org/0004-637X/530/2/625
double CoordinateTransformations::_SG[] =
{ -0.73574257480437488, +0.67726129641389432, +0.0000000000000000,
		-0.07455377836523376, -0.08099147130697673, +0.9939225903997749,
		+0.67314530210920764, +0.73127116581696450, +0.1100812622247821 };

Basic3Vector CoordinateTransformations::convertGalactic2Equatorial(const Basic3Vector& vec,
		Epoque_T epoque)
{
	BasicMatrix R;
	R.setColumnBasedStorage();
	if (epoque == J1950)
	{
		R.use(3, 3, _R1950);
	}
	else if (epoque == J2000)
	{
		R.use(3, 3, _R2000);
	}

	Basic3Vector out = R * vec;
	return out;
}

Basic3Vector CoordinateTransformations::convertEquatorial2Galactic(const Basic3Vector& vec,
		Epoque_T epoque)
{
	BasicMatrix R;
	R.setRowBasedStorage();
	if (epoque == J1950)
	{
		R.use(3, 3, _R1950);
	}
	else if (epoque == J2000)
	{
		R.use(3, 3, _R2000);
	}

	Basic3Vector out = R * vec;
	return out;
}

Basic3Vector CoordinateTransformations::convertEquatorial2SuperGalactic(const Basic3Vector& vec,
		Epoque_T epoque)
{
	Basic3Vector vt = CoordinateTransformations::convertEquatorial2Galactic(vec, epoque);
	return CoordinateTransformations::convertGalactic2SuperGalactic(vt);
}


Basic3Vector CoordinateTransformations::convertSuperGalactic2Equatorial(const Basic3Vector& vec,
			Epoque_T epoque )
{
	Basic3Vector vt = CoordinateTransformations::convertSuperGalactic2Galactic(vec);
	return CoordinateTransformations::convertGalactic2Equatorial(vt, epoque);
}


Basic3Vector CoordinateTransformations::convertGalactic2SuperGalactic(const Basic3Vector& vec)
{
	BasicMatrix R;
	R.setRowBasedStorage();
	R.use(3, 3, _SG);
	Basic3Vector out = R * vec;
	return out;
}

Basic3Vector CoordinateTransformations::convertSuperGalactic2Galactic(const Basic3Vector& vec)
{
	BasicMatrix R;
	R.setColumnBasedStorage();
	R.use(3, 3, _SG);
	Basic3Vector out = R * vec;
	return out;
}

double CoordinateTransformations::timeAngle2Radian(unsigned int hours, unsigned int minutes,
		double seconds)
{
	return (1. / 12. * hours + 1. / 720. * minutes + 1 / 43200. * seconds)
			* M_PI;

}
double CoordinateTransformations:: minutesOfArc2Radian(int degree, int minutes, double seconds)
{
	double s = (degree < 0 ? -1. : 1.);
	return s * (std::abs(degree) + 1. / 60 * minutes + 1. / 3600 * seconds)
			/ 180. * M_PI;
}
}

