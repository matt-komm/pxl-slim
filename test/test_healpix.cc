#include "gtest/gtest.h"
#include "pxl/healpix.hh"
//#include "pxl/astro/HealpixMap.hh"
#include <pxl/astro.hh>
#include <pxl/core.hh>
#include <vector>
#include <algorithm>
#include <test_share.hh>

#include "core_tests/SerializableTests.hh"

using namespace pxl;
TEST(HealpixMap, DefaultConstructor)
{
	HealpixMap h;
	EXPECT_EQ(h.getOrder(),0);
	EXPECT_EQ(h.getNumberOfPixels(),12);
}

TEST(HealpixMap, CopyConstructor)
{
	HealpixMap h(3);
	h.setUserRecord("foo",3.);
	h.setElement(3,4);

	HealpixMap hc = h;
	EXPECT_DOUBLE_EQ(hc.getElement(3),4.);
	EXPECT_TRUE(h.getNumberOfPixels() == hc.getNumberOfPixels());
	EXPECT_TRUE(h.getOrder() == hc.getOrder());
	double foo;
	hc.getUserRecord("foo", foo);
	EXPECT_DOUBLE_EQ(foo, 3);
}


TEST(HealpixMap, CopyInContainer)
{
	HealpixMap *h = new HealpixMap(3);
	BasicContainer b1;
	b1.insertObject(h);

	BasicContainer b2 = b1;
	std::vector<HealpixMap*> v;
	b2.getObjectsOfType<HealpixMap>(v);
	EXPECT_EQ(v.size(), 1);
}



TEST(HealpixMap, PixelDirectionmatching)
{
	HealpixMap h(6);
	for(int i =0; i<h.getNumberOfPixels(); i++)
	{
		Basic3Vector v = h.pixel2Vector(i);
		int j = h.vector2Pixel(v);
		ASSERT_EQ(j,i);
	}
}

class HealpixMapTest : public testing::Test
{
	protected:
		HealpixMap *h;

	virtual void SetUp()
	{
		h = new HealpixMap(6);
	}
	virtual void TearDown()
	{
		delete h;
	}
	void GetNeighborTester(int pixelNumber = 49151)
	{
		std::vector<int> Neighbors;
		h->getNeighbors(pixelNumber, Neighbors);
		Basic3Vector v0 = h->pixel2Vector(pixelNumber);
		// The Neighbors have to be the nearest pixels
		std::vector<double> distances(h->getNumberOfPixels());
		for (int i=0; i < h->getNumberOfPixels(); i++)
		{
			Basic3Vector v1 = h->pixel2Vector(i);
			distances[i] = acos(v1*v0);
		}
		//strip -1 entries

		std::vector<int> NNeighbors;
		for (size_t i=0; i < Neighbors.size(); i++)
		{
			if (Neighbors[i]>-1)
			{
				NNeighbors.push_back(Neighbors[i]);
			}
		}

		std::sort(distances.begin(), distances.end());
		double maxdist = distances[NNeighbors.size()+1];
		for (size_t i=0; i < NNeighbors.size(); i++)
		{
			Basic3Vector v1 = h->pixel2Vector(NNeighbors[i]);
			double d = acos(v1*v0);
			EXPECT_PRED_FORMAT2(::testing::DoubleLE, d, maxdist);
		}
	}

	void GetPixelInConeTester(double x, double y, double z)
	{
		std::vector<int> pixelsInCone;
		Basic3Vector coneCenter(x,y,z);
		double coneRadius = 0.6;
		h->getPixelsInCone(coneCenter, coneRadius, pixelsInCone);

		for(size_t i=0; i < pixelsInCone.size(); i++)
		{
			Basic3Vector v1 = h->pixel2Vector(pixelsInCone[i]);
			double d = acos(v1*coneCenter);
			EXPECT_PRED_FORMAT2(::testing::DoubleLE, d, coneRadius);
		}
	}

};

TEST_F(HealpixMapTest,getNeighbors)
{
	GetNeighborTester();
	GetNeighborTester(0);
	GetNeighborTester(4728);
}

TEST_F(HealpixMapTest, getPixelsInCone)
{
	GetPixelInConeTester(1,0,0);
	GetPixelInConeTester(-1,0,0);
	GetPixelInConeTester(0,1,0);
	GetPixelInConeTester(0,-1,0);
	GetPixelInConeTester(0,0,1);
	GetPixelInConeTester(0,0,-1);
	GetPixelInConeTester (0.66578273111747399, -0.63680021088343564, 0.38886867495925831);
}

TEST(HealpixMap, SerializeDeserialize)
{
	Healpix::initialize();
	HealpixMap h(3);
	h.setElement(3,4);
	HealpixMap* hin = dynamic_cast<HealpixMap*> (ioTest(&h));

	EXPECT_DOUBLE_EQ(hin->getElement(3), h.getElement(3));
	delete hin;
}


//
// Tests for all serializables in astro
typedef Types<pxl::HealpixMap> SerealizableHealpixImplementations;
INSTANTIATE_TYPED_TEST_CASE_P(SerealizablesInHealpix,    // Instance name
                              SerializableTest,             // Test case name
                              SerealizableHealpixImplementations);


