#include "gtest/gtest.h"
#include "pxl/hep.hh"
#include "test_share.hh"

#include "core_tests/SerializableTests.hh"

using namespace pxl;

TEST(SoftRelations, getSoftRelativesOfType)
{
	Particle *a = new Particle();
	Particle *r = new Particle(a);
	r->linkSoft(a, "Foo");

	Event b;
	b.insertObject(a);
	b.insertObject(r);

	std::vector<Particle*> v;
	r->getSoftRelations().getSoftRelativesOfType<Particle>(v, b.getObjectOwner());
	ASSERT_EQ(1, v.size());
	ASSERT_TRUE(v[0] == a);

}

TEST(Serializable, Particle)
{
    pxl::Hep::initialize();
    pxl::Particle p;
    
    pxl::Serializable* ser = ioTest(&p);
	
    pxl::Particle* p_in = dynamic_cast<pxl::Particle*>(ser);
    ASSERT_TRUE(p_in != 0);
    delete p_in;
}

//
// Tests for all serializables in hep 
typedef Types<pxl::Collision, pxl::EventView, pxl::Particle, pxl::Vertex> SerealizableHepImplementations;
INSTANTIATE_TYPED_TEST_CASE_P(SerealizablesInHep,    // Instance name
                              SerializableTest,             // Test case name
                              SerealizableHepImplementations);


