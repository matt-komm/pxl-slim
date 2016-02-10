// HealpixMap.hh, 12.06.10 
//
// This defines the class HealPixMap which provides a very
// basic Hierarchical Equal Area isoLatitude Pixelization (HEALPIX) of a
// sphere, based on the original NASA implementation which is available 
// on http://healpix.jpl.nasa.gov. 
//
// This part of PXL is published unter the GNU GPL v3 as it is based on NASA 
// Healpix, which is published under the GNU GPL v2. It therefore cannot
// be combined with PXL without making the resulting license GPL.
//
// This is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This Software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this software; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

#ifndef PXL_ASTRO_HEALPIXMAP_HH
#define PXL_ASTRO_HEALPIXMAP_HH
#include "pxl/core/macros.hh"

#include <cmath>
#include <stdint.h>
#include <vector>
#include <iostream>
#include "pxl/core/macros.hh"
#include "pxl/core/Basic3Vector.hh"
#include "pxl/core/BasicNVector.hh"
#include "pxl/core/UserRecord.hh"

namespace pxl
{

class PXL_DLL_EXPORT HealpixMap : public BasicNVector, public UserRecordHelper
{
public:

	HealpixMap() : BasicNVector(), _order(0), _npix(0), _ncap(0), _nside(0), _npface(0), _fact1(0), _fact2(0)
	{
		_calculateInternalConstants();
		this->setSize(_npix);
	}

	HealpixMap(const HealpixMap& original) : BasicNVector(original), UserRecordHelper(original), _order(original._order),
		_npix(original._npix), _ncap(original._ncap), _nside(original._nside), _npface(original._npface),
		_fact1(original._fact1), _fact2(original._fact2)
	{
	}

	explicit HealpixMap(const HealpixMap* original) : BasicNVector(original), UserRecordHelper(*original), _order(original->_order),
		_npix(original->_npix), _ncap(original->_ncap), _nside(original->_nside), _npface(original->_npface),
		_fact1(original->_fact1), _fact2(original->_fact2)
	{
	}

	/// Creates a deep copy and returns a C++ pointer to the newly-created object.
	virtual Serializable* clone() const
	{
		return new HealpixMap(*this);
	}

	HealpixMap(int order);

	void serialize(const OutputStream &out) const;
	void deserialize(const InputStream &in);

	///returns the type id of the HealpixMap 
	virtual const Id& getTypeId() const
	{
		return getStaticTypeId();
	}

	///returns the type id of the AstroObject
	static const Id& getStaticTypeId()
	{
		// 9ffee9f7-942d-d259-ea98-6ddd0001f549 was used as Id before
		// Octobre 2013. The numbering system was changed between the two
		// versions; it is now compatible with healpy.
		static const Id id("58488649-3229-4c28-a024-f01a8038cd5d");
		return id;
	}

	int getOrder() const
	{
		return _order;
	}

	int getNumberOfPixels() const
	{
		return _npix;
	}

	static int getNumberOfPixels(unsigned int i);

	/// Returns pixelnumber of pixel 
	int vector2Pixel (const Basic3Vector &vec) const;

	/// Get pixelnumbers of neighbors 
	void getNeighbors(int pix, std::vector<int> &result) const;

	/// Returns Pixels in a cone around Basic3Vector with radius r - this
	/// is the equivalent to healpixbase query_disc
	void getPixelsInCone(const Basic3Vector& coneCenter, double radius, std::vector<int>& listpix) const;

	/// returns a vector pointing to center of the given pixel
	Basic3Vector pixel2Vector(int pixel) const;

	/// sets Vector if pixels match
	void setVector(const BasicNVector &v)
	{
		if (v.getSize() != _npix)
			throw std::runtime_error("Vector size doesnt match number of pixels");
		BasicNVector::operator=(v);
	}

	inline const HealpixMap& operator+=(const HealpixMap& vec)
	{
		BasicNVector::operator+=(vec); return *this;
	}

	inline const HealpixMap& operator-=(const HealpixMap& vec)
	{
		BasicNVector::operator-=(vec); return *this;
	}

	HealpixMap operator-(const HealpixMap&) const;
	HealpixMap operator+(const HealpixMap&) const;

	const HealpixMap& operator*=(double skalar)
	{
		BasicNVector::operator*=(skalar); return *this;
	};
	const HealpixMap& operator/=(double skalar)
	{
		BasicNVector::operator/=(skalar); return *this;
	};

private:
	unsigned int _order; // The Healpix Order (Sometimes refered to as _NSide)
	int _npix;  // The number of pixels
	int _ncap;
	int _nside;
	int _npface;
	double _fact1;
	double _fact2;
	void _calculateInternalConstants();

	// needed for neighbor calculation
	void ring2xyf (int pix, int &ix, int &iy, int &face_num) const;
	int xyf2ring (int ix, int iy, int face_num) const;

	// Returns the remainder of the division v1/v2.
	// The result is non-negative.
	// v1 can be positive or negative; v2 must be positive. 
	double fmodulo (double v1, double v2) const
	{
		return (v1>=0) ? ((v1<v2) ? v1 : std::fmod(v1,v2)) : (fmod(v1,v2)+v2);
	}

	//! Returns the remainder of the division v1/v2.
	/*! The result is non-negative.
	// v1 can be positive or negative; v2 must be positive. */
	template<typename I> I imodulo (I v1, I v2) const 
	{ return (v1>=0) ? ((v1<v2) ? v1 : (v1%v2)) : ((v1%v2)+v2); }

	// Returns the integer n, which fulfills n*n<=arg<(n+1)*(n+1).
	template<typename I> unsigned int isqrt (I arg) const
	{
		using namespace std;
		if (sizeof(I)<=4)
			return unsigned (sqrt(arg+0.5));
		long double arg2 = arg;
		return unsigned (sqrt(arg2+0.5));
	}

	// Returns atan2(y,x) if x!=0 or y!=0; else returns 0.
	double safe_atan2 (double y, double x) const
	{
		using namespace std;
		return ((x==0.) && (y==0.)) ? 0.0 : atan2(y,x);
	}

	// Returns the largest integer which is smaller than (or equal to) arg.
	template<typename I, typename F> I ifloor (F arg) const
	{
		return (arg>=0) ? I(arg) : I(arg)-1;
	}

	void in_ring(int iz, double phi0, double dphi, std::vector<int> &listir) const;

	int ring_above (double z) const;

};

}

#endif
