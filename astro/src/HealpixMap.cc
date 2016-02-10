#include "pxl/astro/HealpixMap.hh"

namespace pxl
{
void HealpixMap::_calculateInternalConstants()
{
	_nside = 1<<_order; // _nside = 2**order
	_npface = _nside << _order; // _nside * 2**order == nside**2
	_ncap = (_npface-_nside)<<1; //(nside**2-nside) * 2**1
	_npix = 12*_npface;
	_fact2 = 4./_npix;
	_fact1 = (_nside<<1)*_fact2; // 2*nside * fact2
}

int HealpixMap::getNumberOfPixels(unsigned int order)
{
	int nside = 1<<order; // _nside = 2**order
	int npface = nside << order; // _nside * 2**order == nside**2
	return 12*npface;
}

HealpixMap::HealpixMap(int order)
{
	_order = order;
	_calculateInternalConstants();
	this->setSize(_npix);
} // HealpixMap

void HealpixMap::serialize(const OutputStream &out) const
{
	BasicNVector::serialize(out);
	UserRecordHelper::serialize(out);
	out.writeUnsignedInt(_order);
}

void HealpixMap::deserialize(const InputStream &in) 
{
	BasicNVector::deserialize(in);
	UserRecordHelper::deserialize(in);
	in.readUnsignedInt(_order);
	_calculateInternalConstants();
}

int HealpixMap::vector2Pixel (const Basic3Vector &vec) const
{
	double z = vec.getZ()/sqrt(vec*vec);
	double phi = safe_atan2(vec.getY(),vec.getX());

	double za = std::fabs(z);
	double tt = fmodulo(phi,2*M_PI) * 2/M_PI; // in [0,4)

	if (za<=2./3.) // Equatorial region
	{
		double temp1 = _nside*(0.5+tt);
		double temp2 = _nside*z*0.75;
		int jp = int(temp1-temp2); // index of ascending edge line
		int jm = int(temp1+temp2); // index of descending edge line

		// ring number counted from z=2/3
		int ir = _nside+ 1 + jp - jm; // in {1,2n+1}
		int kshift = 1-(ir&1); // kshift=1 if ir even, 0 otherwise

		int ip = (jp+jm- _nside+kshift+1)/2; // in {0,4n-1}
		ip = imodulo(ip,4* _nside);

		return _ncap + (ir-1)*4* _nside+ ip;
	}
	else // North & South polar caps
	{
		double tp = tt-int(tt);
		double tmp = _nside*sqrt(3*(1-za));

		int jp = int(tp*tmp); // increasing edge line index
		int jm = int((1.0-tp)*tmp); // decreasing edge line index

		int ir = jp+jm+1; // ring number counted from the closest pole
		int ip = int(tt*ir); // in {0,4*ir-1}
		ip = imodulo(ip,4*ir);

		if (z>0)
		{
			return 2*ir*(ir-1) + ip;
		}
		else
		{
			return _npix - 2*ir*(ir+1) + ip;
		}
	}
} // vector2Pixel 


void HealpixMap::getNeighbors(int pix, std::vector<int> &result) const
{
	result.resize(8);
	static const int xoffset[] = { -1,-1, 0, 1, 1, 1, 0,-1 };
	static const int yoffset[] = {	0, 1, 1, 1, 0,-1,-1,-1 };
	static const int facearray[][12] =
				{ {	8, 9,10,11,-1,-1,-1,-1,10,11, 8, 9 }, // S
					{	5, 6, 7, 4, 8, 9,10,11, 9,10,11, 8 }, // SE
					{ -1,-1,-1,-1, 5, 6, 7, 4,-1,-1,-1,-1 }, // E
					{	4, 5, 6, 7,11, 8, 9,10,11, 8, 9,10 }, // SW
					{	0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11 }, // center
					{	1, 2, 3, 0, 0, 1, 2, 3, 5, 6, 7, 4 }, // NE
					{ -1,-1,-1,-1, 7, 4, 5, 6,-1,-1,-1,-1 }, // W
					{	3, 0, 1, 2, 3, 0, 1, 2, 4, 5, 6, 7 }, // NW
					{	2, 3, 0, 1,-1,-1,-1,-1, 0, 1, 2, 3 } }; // N
	static const int swaparray[][12] =
				{ {	0,0,0,0,0,0,0,0,3,3,3,3 }, // S
					{	0,0,0,0,0,0,0,0,6,6,6,6 }, // SE
					{	0,0,0,0,0,0,0,0,0,0,0,0 }, // E
					{	0,0,0,0,0,0,0,0,5,5,5,5 }, // SW
					{	0,0,0,0,0,0,0,0,0,0,0,0 }, // center
					{	5,5,5,5,0,0,0,0,0,0,0,0 }, // NE
					{	0,0,0,0,0,0,0,0,0,0,0,0 }, // W
					{	6,6,6,6,0,0,0,0,0,0,0,0 }, // NW
					{	3,3,3,3,0,0,0,0,0,0,0,0 } }; // N

	int ix, iy, face_num;
	// We only have the ring scheme!
	//(scheme_==RING) ?
	//	ring2xyf(pix,ix,iy,face_num) : nest2xyf(pix,ix,iy,face_num);
	ring2xyf(pix,ix,iy,face_num);

	// Some undecipherable math happens here
	const int nsm1 = _nside-1;
	if ((ix>0)&&(ix<nsm1)&&(iy>0)&&(iy<nsm1))
	{
		for (int m=0; m<8; ++m)
		{
			result[m] = xyf2ring(ix+xoffset[m],iy+yoffset[m],face_num);
		}
	}
	else
	{
		for (int i=0; i<8; ++i)
		{
			int x=ix+xoffset[i];
			int y=iy+yoffset[i];
			int nbnum=4;
			if (x<0)
				{ x+=_nside; nbnum-=1; }
			else if (x>=_nside)
				{ x-=_nside; nbnum+=1; }
			if (y<0)
				{ y+=_nside; nbnum-=3; }
			else if (y>=_nside)
				{ y-=_nside; nbnum+=3; }

			int f = facearray[nbnum][face_num];
			if (f>=0)
			{
				if (swaparray[nbnum][face_num]&1) x=_nside-x-1;
				if (swaparray[nbnum][face_num]&2) y=_nside-y-1;
				if (swaparray[nbnum][face_num]&4) std::swap(x,y);

				result[i] = xyf2ring(x,y,f);
			}
			else
				result[i] = -1;
		}
	}
} // neighbors


void HealpixMap::getPixelsInCone(const Basic3Vector& coneCenter, double radius, std::vector<int>& listpix) const
{
	double phi = safe_atan2(coneCenter.getY(),coneCenter.getX());
	double theta = acos(coneCenter.getZ());
	listpix.clear();

	double dth1 = _fact2;
	double dth2 = _fact1;
	double cosang = cos(radius);

	double z0 = cos(theta);
	double xa = 1./sqrt((1-z0)*(1+z0));

	double rlat1 = theta - radius;
	double zmax = cos(rlat1);
	int irmin = ring_above (zmax)+1;

	if (rlat1<=0) // north pole in the disc
		for (int m=1; m<irmin; ++m) // rings completely in the disc
			in_ring (m, 0, M_PI, listpix);

	double rlat2 = theta + radius;
	double zmin = cos(rlat2);
	int irmax = ring_above (zmin);

	// ------------- loop on ring number ---------------------
	for (int iz=irmin; iz<=irmax; ++iz) // rings partially in the disc
	{
		double z;
		if (iz<_nside) // north polar cap
			z = 1.0 - iz*iz*dth1;
		else if (iz <= (3*_nside)) // tropical band + equat.
			z = (2*_nside-iz) * dth2;
		else
			z = -1.0 + (4*_nside-iz)*(4*_nside-iz)*dth1;

	// --------- phi range in the disc for each z ---------
		double x = (cosang-z*z0)*xa;
		double ysq = 1-z*z-x*x;
		double dphi=atan2(sqrt(ysq),x);
		in_ring (iz, phi, dphi, listpix);
	}

	if (rlat2>=M_PI)
	{// south pole in the disc
		for (int m=irmax+1; m<(4*_nside); ++m)
		{	// rings completely in the disc
			in_ring (m, 0, M_PI, listpix);
		}
	}
} // getPixelsInCone 


Basic3Vector HealpixMap::pixel2Vector(int pixel) const
{
	double z, phi;

	if (pixel<_ncap) // North Polar cap
	{
		int iring = int(0.5*(1+isqrt(1+2*pixel))); //counted from North pole
		int iphi = (pixel+1) - 2*iring*(iring-1);

		z = 1.0 - (iring*iring)*_fact2;
		phi = (iphi-0.5) * M_PI/2/iring;
	}
	else if (pixel<(_npix-_ncap)) // Equatorial region
	{
		int ip = pixel - _ncap;
		int iring = ip/(4*_nside) + _nside; // counted from North pole
		int iphi = ip%(4*_nside) + 1;
		// 1 if iring+nside is odd, 1/2 otherwise
		double fodd = ((iring+_nside)&1) ? 1 : 0.5;

		int nl2 = 2*_nside;
		z = (nl2-iring)*_fact1;
		phi = (iphi-fodd) * M_PI/nl2;
	}
	else // South Polar cap
	{
		int ip = _npix - pixel;
		int iring = int(0.5*(1+isqrt(2*ip-1))); //counted from South pole
		int iphi = 4*iring + 1 - (ip - 2*iring*(iring-1));

		z = -1.0 + (iring*iring)*_fact2;
		phi = (iphi-0.5) * M_PI/2/iring;
	}

	Basic3Vector res;
	double sintheta = sqrt((1.-z)*(1.+z));
	res.setX(sintheta*cos(phi));
	res.setY(sintheta*sin(phi));
	res.setZ(z);
	return res;
} // pix2vec


void HealpixMap::ring2xyf (int pix, int &ix, int &iy, int &face_num) const
{
	static const int jrll[] = { 2,2,2,2,3,3,3,3,4,4,4,4 };
	static const int jpll[] = { 1,3,5,7,0,2,4,6,1,3,5,7 };

	int iring, iphi, kshift, nr;

	int nl2 = 2*_nside;

	if (pix<_ncap) // North Polar cap
	{
	iring = int(0.5*(1+isqrt(1+2*pix))); //counted from North pole
	iphi = (pix+1) - 2*iring*(iring-1);
	kshift = 0;
	nr = iring;
	face_num=0;
	int tmp = iphi-1;
	if (tmp>=(2*iring))
			{
			face_num=2;
			tmp-=2*iring;
			}
		if (tmp>=iring) ++face_num;
		}
	else if (pix<(_npix-_ncap)) // Equatorial region
		{
		int ip = pix - _ncap;
    iring = (ip>>(_order+2)) + _nside; // counted from North pole
    iphi = (ip&(4*_nside-1)) + 1;
		kshift = (iring+_nside)&1;
		nr = _nside;
		unsigned int ire = iring-_nside+1;
		unsigned int irm = nl2+2-ire;
		int ifm, ifp;
    ifm = (iphi - ire/2 + _nside -1) >> _order;
    ifp = (iphi - irm/2 + _nside -1) >> _order;
		
		if (ifp == ifm) // faces 4 to 7
			face_num = (ifp==4) ? 4 : ifp+4;
		else if (ifp<ifm) // (half-)faces 0 to 3
			face_num = ifp;
		else // (half-)faces 8 to 11
			face_num = ifm + 8;
		}
	else // South Polar cap
		{
		int ip = _npix - pix;
		iring = int(0.5*(1+isqrt(2*ip-1))); //counted from South pole
		iphi = 4*iring + 1 - (ip - 2*iring*(iring-1));
		kshift = 0;
		nr = iring;
		iring = 2*nl2-iring;
		face_num=8;
		int tmp = iphi-1;
		if (tmp>=(2*nr))
			{
			face_num=10;
			tmp-=2*nr;
			}
		if (tmp>=nr) ++face_num;
		}

	int irt = iring - (jrll[face_num]*_nside) + 1;
	int ipt = 2*iphi- jpll[face_num]*nr - kshift -1;
	if (ipt>=nl2) ipt-=8*_nside;

	ix = (ipt-irt) >>1;
	iy =(-(ipt+irt))>>1;
} //ring2xyf


int HealpixMap::xyf2ring (int ix, int iy, int face_num) const
{

	static const int jrll[] = { 2,2,2,2,3,3,3,3,4,4,4,4 };
	static const int jpll[] = { 1,3,5,7,0,2,4,6,1,3,5,7 };

	int nl4 = 4*_nside;
	int jr = (jrll[face_num]*_nside) - ix - iy - 1;

	int nr, kshift, n_before;
	if (jr<_nside)
		{
		nr = jr;
		n_before = 2*nr*(nr-1);
		kshift = 0;
		}
	else if (jr > 3*_nside)
		{
		nr = nl4-jr;
		n_before = _npix - 2*(nr+1)*nr;
		kshift = 0;
		}
	else
		{
		nr = _nside;
		n_before = _ncap + (jr-_nside)*nl4;
		kshift = (jr-_nside)&1;
		}

	int jp = (jpll[face_num]*nr + ix - iy + 1 + kshift) / 2;
	if (jp>nl4)
		jp-=nl4;
	else
		if (jp<1) jp+=nl4;

	return n_before + jp - 1;
} //xyf2ring


void HealpixMap::in_ring(int iz, double phi0, double dphi, std::vector<int> &listir) const
{
	int nr, ir, ipix1;
	double shift=0.5;

	if (iz<_nside) // north pole
	{
		ir = iz;
		nr = ir*4;
		ipix1 = 2*ir*(ir-1);//		lowest pixel number in the ring
	}
	else if (iz>(3*_nside)) // south pole
	{
		ir = 4*_nside - iz;
		nr = ir*4;
		ipix1 = _npix - 2*ir*(ir+1); // lowest pixel number in the ring
	}
	else // equatorial region
	{
		ir = iz - _nside + 1; //		within {1, 2*nside + 1}
		nr = _nside*4;
		if ((ir&1)==0) shift = 0;
		ipix1 = _ncap + (ir-1)*nr; // lowest pixel number in the ring
	}
	int ipix2 = ipix1 + nr - 1;//		highest pixel number in the ring


	// ----------- constructs the pixel list --------------
	if (dphi > (M_PI-1e-7))
	{
		for (int i=ipix1; i<=ipix2; ++i)
		{
			listir.push_back(i);
		}
	}
	else
	{
		int ip_lo = ifloor<int>(nr*0.5 / M_PI*(phi0-dphi) - shift)+1;
		int ip_hi = ifloor<int>(nr*0.5 / M_PI*(phi0+dphi) - shift);
		int pixnum = ip_lo+ipix1;
		if (pixnum<ipix1)
		{
			pixnum += nr;
		}

		for (int i=ip_lo; i<=ip_hi; ++i, ++pixnum)
		{
			if (pixnum>ipix2)
			{
				pixnum -= nr;
			}
			listir.push_back(pixnum);
		}
	}
} // end ring


int HealpixMap::ring_above (double z) const
{
	double az=std::fabs(z);
	if (az>2./3) // polar caps
		{
		int iring = int(_nside*sqrt(3*(1-az)));
		return (z>0) ? iring : 4*_nside-iring-1;
		}
	else // ----- equatorial region ---------
		return int(_nside*(2-1.5*z));
}

HealpixMap HealpixMap::operator-(const HealpixMap& vec) const
{
	HealpixMap out(*this);
	out -= vec;
	return out;
}

HealpixMap HealpixMap::operator+(const HealpixMap& vec) const
{
	HealpixMap out(*this);
	out += vec;
	return out;
}

} // namespace pxl
