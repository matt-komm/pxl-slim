//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#ifndef PXL_ASTRO_UHECR_HH
#define PXL_ASTRO_UHECR_HH
#include "pxl/core/macros.hh"

#include "pxl/astro/AstroObject.hh"
#include <iostream>

namespace pxl
{
/**
 A class for Ultra-High-Energy Cosmic Rays, with energy, mass and charge
 */

class PXL_DLL_EXPORT UHECR : public AstroObject
{
public:

	UHECR() : AstroObject(), _energy(0.), _denergy(0.), _mass(0.), _charge(0.)
	{
	}

	UHECR(const UHECR& original) :
	AstroObject(original), _energy(original._energy), _denergy(original._denergy), _mass(original._mass), _charge(original._charge)
	{
	}

	explicit UHECR(const UHECR* original) :
	AstroObject(original), _energy(original->_energy), _denergy(original->_denergy), _mass(original->_mass), _charge(original->_charge)
	{
	}

	UHECR(double longitude, double latitude, double energy=0.) : AstroObject(longitude, latitude), _energy(energy)
	{
	}

	UHECR(const Basic3Vector& vector, double energy=0.) : AstroObject(vector), _energy(energy)
	{
	}


	virtual ~UHECR()
	{
	}

	virtual void serialize(const OutputStream &out) const;

	virtual void deserialize(const InputStream &in);

	virtual const Id& getTypeId() const
	{
		return getStaticTypeId();
	}

	static const Id& getStaticTypeId()
	{
		static const Id id("a4d8433f-fa90-06e9-9c28-8d784974c1f9");
		return id;
	}

	/// Creates a deep copy and returns a C++ pointer to the newly-created object.
	virtual Serializable* clone() const
	{
		return new UHECR(*this);
	}

	/// Prints out some information about the CR
	virtual std::ostream& print(int level=1, std::ostream& os=std::cout, int pan=1) const;

	inline double getEnergyError() const
	{
		return _denergy;
	}

	inline double getEnergy() const
	{
		return _energy;
	}

	inline double getCharge() const
	{
		return _charge;
	}

	inline double getMass() const
	{
		return _mass;
	}

	inline void setEnergy(double energy)
	{
		_energy= energy;
	}

	inline void setEnergyError(double denergy)
	{
		_denergy= denergy;
	}

	inline void setMass(double mass)
	{
		_mass = mass;
	}

	inline void setCharge(double charge)
	{
		_charge = charge;
	}

private:
	double _energy;
	double _denergy;
	double _mass;
	double _charge;

	UHECR& operator=(const UHECR& original)
	{
		return *this;
	}
};
}
//namespace pxl

#endif
