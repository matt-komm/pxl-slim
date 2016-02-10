#include "gtest/gtest.h"
#include "pxl/astro.hh"
#include "test_share.hh"

#include "core_tests/SerializableTests.hh"

using namespace pxl;
TEST(AstroBasicObject, DefaultConstructor)
{
	const AstroBasicObject a;
	Basic3Vector v = a.getGalacticDirectionVector();
	EXPECT_DOUBLE_EQ(v.getX(), 1);
	EXPECT_DOUBLE_EQ(v.getY(), 0);
	EXPECT_DOUBLE_EQ(v.getZ(), 0);
}

TEST(AstroBasicObject, angularDistance)
{
	AstroBasicObject a;
	a.setGalacticXYZ(1,0,0);
	AstroBasicObject b;
	b.setGalacticXYZ(0,1,0);
	EXPECT_DOUBLE_EQ(a.angularDistanceTo(b), M_PI/2);
	EXPECT_DOUBLE_EQ(a.angularDistanceTo(&b), M_PI/2);

	a.setGalacticXYZ(1,0,0);
	b.setGalacticXYZ(-1,0,0);
	EXPECT_DOUBLE_EQ(a.angularDistanceTo(b), M_PI);

	a.setGalacticXYZ(1,0,0);
	b.setGalacticXYZ(1,0,0);
	EXPECT_DOUBLE_EQ(a.angularDistanceTo(b), 0);

}

TEST(AstroBasicObject, TestCoordinateSettersAndGetters)
{
	AstroBasicObject a;
	a.setGalacticCoordinates(1.2,0.6);
	EXPECT_DOUBLE_EQ(a.getGalacticLongitude(), 1.2);
	EXPECT_DOUBLE_EQ(a.getGalacticLatitude(), 0.6);
	EXPECT_TRUE(a.getGalacticDirectionVector().isUnitVector());
	EXPECT_TRUE(a.getSuperGalacticDirectionVector().isUnitVector());

	a.setSuperGalacticCoordinates(-0.2,-0.7);
	EXPECT_DOUBLE_EQ(a.getSuperGalacticLongitude(), -0.2);
	EXPECT_DOUBLE_EQ(a.getSuperGalacticLatitude(), -0.7);
	EXPECT_TRUE(a.getGalacticDirectionVector().isUnitVector());

	a.setEquatorialCoordinates(0.2,1.2);
	EXPECT_NEAR(a.getRightAscension(), 0.2,1E-6);
	EXPECT_NEAR(a.getDeclination(), 1.2,1E-6);

	EXPECT_TRUE(a.getGalacticDirectionVector().isUnitVector());
	EXPECT_TRUE(a.getSuperGalacticDirectionVector().isUnitVector());

	pxl::Basic3Vector v(1,0,0);
	a.setGalacticDirectionVector(v);
	EXPECT_TRUE(v == a.getGalacticDirectionVector());
}

TEST(AstroBasicObject, SerializeDeserialize)
{
	Astro::initialize();
	AstroBasicObject a;
	a.setGalacticCoordinates(1.2,0.6);
	AstroBasicObject *b = dynamic_cast<AstroBasicObject*> (ioTest(&a));
	EXPECT_NEAR(a.getGalacticLongitude(), b->getGalacticLongitude(),1E-6);
	EXPECT_NEAR(a.getGalacticLatitude(), b->getGalacticLatitude(),1E-6);
	delete b;
}

TEST(AstroObject, softRelations)
{
	AstroObject a;
	AstroObject b;
	a.linkSoft(&b, "foo");
	EXPECT_TRUE(a.getSoftRelations().has(&b,"foo"));
	EXPECT_TRUE(b.getSoftRelations().has(&a,"foo"));
	EXPECT_FALSE(a.getSoftRelations().has(&b,"bar"));
	a.unlinkSoft(b,"foo");
	EXPECT_FALSE(a.getSoftRelations().has(&b,"foo"));
	EXPECT_FALSE(b.getSoftRelations().has(&a,"foo"));
}


TEST(AstroObject, UniqueSoftrelations)
{
	pxl::AstroObject r1,r2;
	r1.linkSoft(r2, "Foo");
	r1.linkSoft(r2, "Foo");
	r2.linkSoft(r1, "Foo");
	r2.linkSoft(r1, "Foo");
	EXPECT_EQ(r1.getSoftRelations().size(),1);
}


TEST(RegionOfInterest, objectInCone)
{
	RegionOfInterest roi;
	roi.setGalacticCoordinates(1,0);
	roi.setConeRadius(0.15);

	AstroBasicObject a;
	a.setGalacticCoordinates(1.1,0);
	AstroBasicObject b;
	b.setGalacticCoordinates(1.2,0);
	EXPECT_TRUE(roi.objectInCone(a));
	EXPECT_FALSE(roi.objectInCone(b));

	roi.setGalacticCoordinates(0,M_PI/2);
	a.setGalacticCoordinates(0,M_PI/2);
	EXPECT_TRUE(roi.objectInCone(a));
	EXPECT_FALSE(roi.objectInCone(b));

	roi.setGalacticCoordinates(0,-M_PI/2);
	a.setGalacticCoordinates(0,-M_PI/2);
	b.setGalacticCoordinates(0,M_PI/2);
	EXPECT_TRUE(roi.objectInCone(a));
	EXPECT_FALSE(roi.objectInCone(b));
}

TEST(RegionOfInterest, linkIfObjectInCone)
{
	RegionOfInterest roi;
	roi.setGalacticCoordinates(1,0);
	roi.setConeRadius(0.15);

	AstroObject a;
	a.setGalacticCoordinates(1.1,0);
	AstroObject b;
	b.setGalacticCoordinates(1.2,0);
	roi.linkIfObjectInCone(a);
	roi.linkIfObjectInCone(b);
	EXPECT_TRUE(roi.getSoftRelations().has(&a));
	EXPECT_TRUE(a.getSoftRelations().has(&roi));
	EXPECT_FALSE(roi.getSoftRelations().has(&b));
	EXPECT_FALSE(b.getSoftRelations().has(&roi));
}

TEST(RegionOfInterest, SerializeDeserialize)
{
	Astro::initialize();
	RegionOfInterest a;
	a.setConeRadius(0.4);
	RegionOfInterest *b = dynamic_cast<RegionOfInterest*> (ioTest(&a));
	EXPECT_TRUE(a.getConeRadius() == b->getConeRadius());
	delete b;
}

TEST(UHECR, SerializeDeserialize)
{
	Astro::initialize();
	UHECR a;
	a.setEnergy(0.4);
	a.setMass(0.3);
	a.setEnergyError(4);
	a.setCharge(40);

	UHECR *b = dynamic_cast<UHECR*> (ioTest(&a));
	EXPECT_TRUE(a.getEnergy() == b->getEnergy());
	EXPECT_TRUE(a.getEnergyError() == b->getEnergyError());
	EXPECT_TRUE(a.getMass() == b->getMass());
	EXPECT_TRUE(a.getCharge() == b->getCharge());
	delete b;
}

TEST(UHECRSource, SerializeDeserialize)
{
	Astro::initialize();
	UHECRSource a;
	a.setName("CenB");
	a.setDistance(0.4);
	a.setLuminosity(1.3);

	UHECRSource *b = dynamic_cast<UHECRSource*> (ioTest(&a));
	EXPECT_TRUE(a.getDistance() == b->getDistance());
	EXPECT_TRUE(a.getLuminosity() == b->getLuminosity());
	EXPECT_STREQ(a.getName().c_str(), b->getName().c_str());
	delete b;
}

class CoordinatesTester : public testing::Test
{
	protected:
		void TestCoordinatesObject(std::string name, double longitude, double latitude, double rightAscension, double declination, double superGalacticLongitude, double superGalacticLatitude, double accuracy, CoordinateTransformations::Epoque_T epoque=CoordinateTransformations::J2000)
		{
			if (longitude > M_PI)
			{
				longitude-=2*M_PI;
			}
			if (superGalacticLongitude > M_PI)
			{
				superGalacticLongitude-=2*M_PI;
			}

			AstroBasicObject aGalactic;
			AstroBasicObject aSuperGalactic;
			AstroBasicObject aEquatorial;

			aGalactic.setGalacticCoordinates(longitude,latitude);
			aSuperGalactic.setSuperGalacticCoordinates(superGalacticLongitude, superGalacticLatitude);
			aEquatorial.setEquatorialCoordinates(rightAscension, declination, epoque);

			EXPECT_NEAR(aGalactic.getSuperGalacticLongitude(), superGalacticLongitude, accuracy);
			EXPECT_NEAR(aGalactic.getSuperGalacticLatitude(), superGalacticLatitude, accuracy);
			EXPECT_NEAR(aGalactic.getRightAscension(), rightAscension, accuracy);
			EXPECT_NEAR(aGalactic.getDeclination(), declination, accuracy);

			EXPECT_NEAR(aSuperGalactic.getGalacticLongitude(), longitude, accuracy);
			EXPECT_NEAR(aSuperGalactic.getGalacticLatitude(), latitude, accuracy);
			EXPECT_NEAR(aSuperGalactic.getRightAscension(), rightAscension, accuracy);
			EXPECT_NEAR(aSuperGalactic.getDeclination(), declination, accuracy);

			EXPECT_NEAR(aEquatorial.getSuperGalacticLongitude(), superGalacticLongitude, accuracy);
			EXPECT_NEAR(aEquatorial.getSuperGalacticLatitude(), superGalacticLatitude, accuracy);
			EXPECT_NEAR(aEquatorial.getGalacticLongitude(), longitude, accuracy);
			EXPECT_NEAR(aEquatorial.getGalacticLatitude(), latitude, accuracy);

			EXPECT_PRED_FORMAT2(::testing::DoubleLE, aGalactic.angularDistanceTo(aSuperGalactic), accuracy);
			EXPECT_PRED_FORMAT2(::testing::DoubleLE, aGalactic.angularDistanceTo(aEquatorial), accuracy);
			EXPECT_PRED_FORMAT2(::testing::DoubleLE, aSuperGalactic.angularDistanceTo(aEquatorial), accuracy);
		}
};


TEST_F(CoordinatesTester, CoordinateTransformations)
{
		TestCoordinatesObject("Galactic Center",
		0,
		0,
		CoordinateTransformations::timeAngle2Radian(17,45,37.224),
		CoordinateTransformations::minutesOfArc2Radian(-28,56,10.23),
		-174.21384/180.*M_PI,
		42.3102/180.*M_PI,
		1E-5,
		CoordinateTransformations::J2000
		);

		TestCoordinatesObject("Virgo Cluster",
		(279.676-360)/180.*M_PI,
		74.460/180*M_PI,
		CoordinateTransformations::timeAngle2Radian(12,26,34),
		CoordinateTransformations::minutesOfArc2Radian(12,43,00),
		102.260/180*M_PI,
		-3.254/180*M_PI,
		1E-3,
		CoordinateTransformations::J2000
		);
//Objects from the 2 Micron All-Sky Survey Redshift Survey
//2MRS, Huchra et al. 2007
//http://www.cfa.harvard.edu/~huchra/seminar/lsc/
		TestCoordinatesObject("000138.30+2329011 ",
		109.092/180.*M_PI,
		-38.111/180*M_PI,
		CoordinateTransformations::timeAngle2Radian(00,01,38.30),
		CoordinateTransformations::minutesOfArc2Radian(23,29,1.1),
		(316.635-360)/180*M_PI,
		17.611/180*M_PI,
		0.1,
		CoordinateTransformations::J2000
		);


		TestCoordinatesObject("115514.64-3741464",
		(291.318-360.)/180.*M_PI,
		23.972/180*M_PI,
		178.811/180.*M_PI,
		-37.696/180.*M_PI,
		(151.377)/180*M_PI,
		-20.741/180*M_PI,
		0.1,
		CoordinateTransformations::J2000
		);

		TestCoordinatesObject("235925.50+2044595",
		(291.318-360.)/180.*M_PI,
		23.972/180*M_PI,
		178.811/180.*M_PI,
		-37.696/180.*M_PI,
		(151.377)/180*M_PI,
		-20.741/180*M_PI,
		0.1,
		CoordinateTransformations::J2000
		);


}

TEST(CoordinateTransformations, equatorialEpoques)
{
//data from VCV Catloguei
// test MS 23575-6352
AstroBasicObject j2000;
AstroBasicObject j1950;

j2000.setEquatorialCoordinates(0, 0, 8.0, -63,35,36,CoordinateTransformations::J2000);
j1950.setEquatorialCoordinates(23, 57 , 33.5, -63, 52, 18,CoordinateTransformations::J1950);
EXPECT_NEAR(j2000.angularDistanceTo(j1950), 0,0.01);

}


TEST(SoftRelations, getSoftRelativesOfType)
{
	AstroObject *a = new AstroObject();
	RegionOfInterest *r = new RegionOfInterest();
	r->linkSoft(a, "Foo");

	BasicContainer b;
	b.insertObject(a);
	b.insertObject(r);
	Serializable* f = b.getById(a->getId());
	ASSERT_EQ(f,a);
	Serializable* rf = b.getById(r->getId());
	ASSERT_EQ(rf,r);

	std::vector<AstroObject*> v;
	r->getSoftRelations().getSoftRelativesOfType<AstroObject>(v, b);
	ASSERT_EQ(1, v.size());
	ASSERT_TRUE(v[0] == a);

}




//
// Tests for all serializables in astro
typedef Types<pxl::AstroBasicObject, pxl::AstroObject, pxl::UHECR, pxl::UHECRSource, pxl::RegionOfInterest> SerealizableAstroImplementations;
INSTANTIATE_TYPED_TEST_CASE_P(SerealizablesInAstro,    // Instance name
                              SerializableTest,             // Test case name
                              SerealizableAstroImplementations);


