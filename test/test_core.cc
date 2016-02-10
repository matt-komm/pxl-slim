#include "gtest/gtest.h"
#include "pxl/core.hh"

#include <stdlib.h>

#include "test_share.hh"

// Tests defined for all Serializables
#include "core_tests/SerializableTests.hh"

TEST(functions, trim)
{
	std::string s = " a ";
	s = pxl::trim(s);
	EXPECT_TRUE(s == "a");
}

TEST(functions, expandEnvironmentVariables)
{
#ifdef WIN32
	EXPECT_TRUE(pxl::expandEnvironmentVariables("%USERPROFILE%") == ::getenv("USERPROFILE"));
#else
	EXPECT_TRUE(pxl::expandEnvironmentVariables("$HOME") == ::getenv("HOME"));
#endif

}

TEST(Variant, Vector)
{
	pxl::Variant a;
	std::vector<pxl::Variant> &v = a;
	v.resize(2);
	v[0] = 2;
	v[1] = "This is a string";
	std::cout << a << std::endl;

	pxl::Variant s;
	s = "This is a string too";
	std::cout << s << std::endl;

	pxl::Variant dbl = pxl::Variant::fromDouble(5.0);
	float flt = dbl;
	EXPECT_FLOAT_EQ(5.0, flt);

	pxl::Variant str = pxl::Variant::fromString("5.0");
	flt = str;
	EXPECT_FLOAT_EQ(5.0, flt);

	pxl::Object obj;
	double delta_R(0.0);
	obj.setUserRecord("min_deltaR_probe_jet", delta_R);
	obj.setUserRecord("num_jets_selected", "5");
	obj.setUserRecord("test_string", "test_string_content");

	float n_jets(99999.);
	float delta_r(99999.);
	float dummy(99999.);
	obj.getUserRecord("num_jets_selected", n_jets);
	obj.getUserRecord("min_deltaR_probe_jet", delta_r);
	obj.getUserRecord("dummy", dummy);

	EXPECT_FLOAT_EQ(5, n_jets);
	EXPECT_FLOAT_EQ(0, delta_r);
	EXPECT_FLOAT_EQ(99999, dummy);

	std::string test_string;
	obj.getUserRecord("test_string", test_string);
	EXPECT_EQ("test_string_content", test_string);
}

//#ifdef PXL_ENABLE_SFTP
//#ifdef PXL_ENABLE_SFTP_FAILS
//TEST(FileImpl, sFTPFileImpl)
//{
//	pxl::File file;
//	EXPECT_TRUE(file.open("ssh://portal.physik.rwth-aachen.de/.bashrc"));
//	char buffer[1024];
//	file.read(buffer, 1024);
//	printf(buffer);
//}
//#endif

TEST(Stream, Buffered)
{
	pxl::BufferOutput output;
	output.writeBool(true);
	output.writeChar('Z');
	output.writeDouble(3.14);
	output.writeFloat(0.1);
	output.writeInt(5);
	output.writeLong(100000000);
	output.writeShort(2);
	output.writeString("Hello World!");
	output.writeUnsignedChar(230);
	output.writeUnsignedInt(3000000000);
	output.writeUnsignedLong(3100000000);
	output.writeUnsignedShort(40000);

	pxl::Id id;
	id.serialize(output);

	pxl::BasicContainer container;
	container.setUserRecord("test", "test");
	container.serialize(output);

	pxl::BufferInput input;
	input.buffer = output.buffer;
	bool b;
	input.readBool(b);
	EXPECT_TRUE(b);

	char c;
	input.readChar(c);
	EXPECT_EQ('Z', c);

	double d;
	input.readDouble(d);
	EXPECT_DOUBLE_EQ(3.14, d);

	float f;
	input.readFloat(f);
	EXPECT_FLOAT_EQ(0.1, f);

	int i;
	input.readInt(i);
	EXPECT_EQ(5, i);

	long l;
	input.readLong(l);
	EXPECT_EQ(100000000, l);

	short s;
	input.readShort(s);
	EXPECT_EQ(2, s);

	std::string str;
	input.readString(str);
	EXPECT_EQ(std::string("Hello World!"), str);

	unsigned char uc;
	input.readUnsignedChar(uc);
	EXPECT_EQ(230, uc);

	unsigned int ui;
	input.readUnsignedInt(ui);
	EXPECT_EQ(3000000000, ui);

	unsigned long ul;
	input.readUnsignedLong(ul);
	EXPECT_EQ(3100000000, ul);

	unsigned short us;
	input.readUnsignedShort(us);
	EXPECT_EQ(40000, us);

	pxl::Id id2(input);
	EXPECT_EQ(id, id2);

	pxl::Id type2(input);
	pxl::BasicContainer container2;
	container2.deserialize(input);
	EXPECT_EQ(std::string("test"), container2.getUserRecord("test").asString());
}

TEST(Basic3Vector, DefaultConstructor)
{
	const pxl::Basic3Vector b;
	ASSERT_DOUBLE_EQ(b.getX(), 0);
	ASSERT_DOUBLE_EQ(b.getY(), 0);
	ASSERT_DOUBLE_EQ(b.getZ(), 0);
}

TEST(Basic3Vector, ConstructFromValues)
{
	const pxl::Basic3Vector b(1, 2, 3);
	ASSERT_DOUBLE_EQ(b.getX(), 1);
	ASSERT_DOUBLE_EQ(b.getY(), 2);
	ASSERT_DOUBLE_EQ(b.getZ(), 3);

	double v[] =
	{ 3, 4, 5 };
	const pxl::Basic3Vector c(v);

	ASSERT_DOUBLE_EQ(c.getX(), 3);
	ASSERT_DOUBLE_EQ(c.getY(), 4);
	ASSERT_DOUBLE_EQ(c.getZ(), 5);
}

TEST(Basic3Vector, testGetAngleTo)
{
	pxl::Basic3Vector v1(1, 0, 0);
	pxl::Basic3Vector v2(0, 1, 0);
	pxl::Basic3Vector v3(0, 1, 1);
	EXPECT_DOUBLE_EQ(v1.getAngleTo(v2), M_PI/2);
	EXPECT_DOUBLE_EQ(v2.getAngleTo(v1), M_PI/2);
	EXPECT_DOUBLE_EQ(v1.getAngleTo(v3), M_PI/2);
	EXPECT_DOUBLE_EQ(v2.getAngleTo(v3), M_PI/4);
	EXPECT_DOUBLE_EQ(v1.getAngleTo(v1), 0);
	EXPECT_DOUBLE_EQ(v1.getAngleTo(-v1), M_PI);
}

TEST(Basic3Vector, CopyConstructor)
{
	const pxl::Basic3Vector b(1, 2, 3);
	const pxl::Basic3Vector c(b);
	ASSERT_DOUBLE_EQ(c.getX(), 1);
	ASSERT_DOUBLE_EQ(c.getY(), 2);
	ASSERT_DOUBLE_EQ(c.getZ(), 3);
	ASSERT_EQ(c, b);
}

TEST(Basic3Vector, ExplicitCopyConstructor)
{
	pxl::Basic3Vector *b = new pxl::Basic3Vector(1, 2, 3);
	pxl::Basic3Vector c(b);
	ASSERT_DOUBLE_EQ(c.getX(), 1);
	ASSERT_DOUBLE_EQ(c.getY(), 2);
	ASSERT_DOUBLE_EQ(c.getZ(), 3);
	delete b;
}

TEST(Basic3Vector, EqualOperator)
{
	const pxl::Basic3Vector b(1, 2, 3);
	const pxl::Basic3Vector c(b);
	const pxl::Basic3Vector d(3, 2, 3);
	ASSERT_TRUE(b == c);
	ASSERT_FALSE(b != c);
	ASSERT_TRUE(b != d);
	ASSERT_FALSE(b == d);
}

TEST(Basic3Vector, isUnitVector)
{
	pxl::Basic3Vector a(1, 0, 0);
	pxl::Basic3Vector b(0, 2, 0);

	EXPECT_TRUE(a.isUnitVector());
	EXPECT_FALSE(b.isUnitVector());
	a *= 2;
	b /= 2;
	EXPECT_FALSE(a.isUnitVector());
	EXPECT_TRUE(b.isUnitVector());
}

TEST(Basic3Vector, CrossProduct)
{
	pxl::Basic3Vector ex(1, 0, 0);
	pxl::Basic3Vector ey(0, 1, 0);
	pxl::Basic3Vector ez(0, 0, 1);

	EXPECT_TRUE(ex.cross(ey) == ez);
}

TEST(Basic3Vector, norm)
{
	pxl::Basic3Vector v(1, 2, 3);
	EXPECT_NEAR(v.norm(1), 6.0, 0.00000000001);
	EXPECT_NEAR(v.norm(2), 3.742, 0.001);
	EXPECT_NEAR(v.norm(3), 3.302, 0.001);
	EXPECT_NEAR(v.norm(4), 3.146, 0.001);
	EXPECT_NEAR(v.norm(-1), 3.0, 0.0000000001);
	EXPECT_THROW(v.norm(0), std::runtime_error);
}

TEST(Basic3Vector, rotate)
{
	pxl::Basic3Vector ex(1, 0, 0);
	pxl::Basic3Vector ey(0, 1, 0);
	pxl::Basic3Vector ez(0, 0, 1);

	ex.rotate(ez, M_PI / 2);
	EXPECT_EQ(ex, ey);
}

class BasicNVectorTest: public testing::Test
{
protected:
	pxl::BasicNVector v0;
	pxl::BasicNVector v1;
	pxl::BasicNVector v2;
	pxl::BasicNVector n;

	virtual void SetUp()
	{
		v0.setSize(3);
		v1.setSize(3);
		v2.setSize(3);
		n.setSize(3);
		for (int i = 0; i < 3; i++)
		{
			v1(i) = i;
			v2(i) = i;
			n(i) = i + 1;
		}
	}
};

TEST_F(BasicNVectorTest, setSizeSafety)
{
	v0.use(v1.getSize(), v1.getArray());
	EXPECT_THROW(v0.setSize(4), std::runtime_error);
	pxl::BasicNVector v;
	v0 = v;

	v1.setElement(1, 1);
	v1.setSize(5);
	EXPECT_EQ(v1.getElement(1), 1);
	EXPECT_EQ(v1.getElement(4), 0);

}

TEST_F(BasicNVectorTest, EqualOperator)
{
	ASSERT_TRUE(v1 == v2);
	ASSERT_FALSE(v1 != v2);

	ASSERT_TRUE(v0 != v2);
	ASSERT_FALSE(v0 == v2);
}

TEST_F(BasicNVectorTest, ScalarProduct)
{
	EXPECT_DOUBLE_EQ(v1 * v2, 5);
}

TEST_F(BasicNVectorTest, norm)
{
	// see: http://mathworld.wolfram.com/VectorNorm.html
	EXPECT_NEAR(n.norm(1), 6.0, 0.00000000001);
	EXPECT_NEAR(n.norm(2), 3.742, 0.001);
	EXPECT_NEAR(n.norm(3), 3.302, 0.001);
	EXPECT_NEAR(n.norm(4), 3.146, 0.001);
	EXPECT_NEAR(n.norm(-1), 3.0, 0.0000000001);
	EXPECT_THROW(n.norm(0), std::runtime_error);
}

TEST_F(BasicNVectorTest, MathOperators)
{
	EXPECT_TRUE(v1 - v2 == v0);
	EXPECT_TRUE(v1 + v2 == 2 * v1);

	v1 -= v2;
	EXPECT_TRUE(v1 == v0);
	v1 += v2;
	EXPECT_TRUE(v1 == v2);

	v0 = 0.5 * v2 + 0.5 * v1;
	EXPECT_TRUE(v1 == v0);

	v0 /= 0.5;

	EXPECT_TRUE(v1 + v2 == v0);
}

TEST_F(BasicNVectorTest, SerializeDeserialize)
{
	pxl::Core::initialize();
	pxl::BasicNVector *v3 = dynamic_cast<pxl::BasicNVector*>(ioTest(&v1));
	ASSERT_TRUE(*v3 == v1);
	delete v3;
}

TEST(BasicMatrix, EqualOperator)
{
	pxl::BasicMatrix A(5, 5);
	pxl::BasicMatrix B(5, 5);
	pxl::BasicMatrix C(5, 5);
	for (size_t i = 0; i < 5; i++)
	{
		for (size_t j = 0; j < 5; j++)
		{
			A(i, j) = i * 5 + j;
			B(i, j) = i * 5 + j;
		}
	}
	ASSERT_TRUE(A == B);
	ASSERT_FALSE(A != B);

	ASSERT_TRUE(A != C);
	ASSERT_FALSE(A == C);
}

TEST(BasicMatrix, MatrixVectorOperations)
{
	pxl::BasicNVector N1(3);
	N1(1) = 1;
	pxl::Basic3Vector v1(2, 1, 3);

	pxl::BasicMatrix E(3, 3);
	for (size_t i = 0; i < 3; i++)
	{
		E(i, i) = 1;
	}
	ASSERT_TRUE(E * N1 == N1);
	ASSERT_TRUE(E * v1 == v1);
}

TEST(RotationMatrix, Rotation)
{
	pxl::Basic3Vector ex(1, 0, 0);
	pxl::Basic3Vector ey(0, 1, 0);
	pxl::Basic3Vector ez(0, 0, 1);
	{
		pxl::RotationMatrix R(ez, M_PI / 2);
		ASSERT_TRUE(R * ex == ey);
	}

	{
		pxl::RotationMatrix R(ex, M_PI / 2);
		ASSERT_TRUE(R * ey == ez);
	}

	{
		pxl::RotationMatrix R(ey, M_PI / 2);
		ASSERT_TRUE(R * ez == ex);
	}

	{
		pxl::RotationMatrix R(ez, 0.1);
		pxl::Basic3Vector t = R * ex;
		EXPECT_TRUE(t.isUnitVector());
		EXPECT_DOUBLE_EQ(t * ex, cos(0.1));
		EXPECT_DOUBLE_EQ(t * ez, 0);
	}

}

class LorentzVectorTest: public testing::Test
{
protected:
	pxl::LorentzVector test_vector;
	pxl::LorentzVector daughter1;
	pxl::LorentzVector daughter2;
	virtual void SetUp()
	{
		daughter1.setXYZ(400., 300., 200.);
		daughter1.setT(1000.);

		daughter2.setXYZ(444., 333., 222.);
		daughter2.setT(1111.);
	}
};

TEST_F(LorentzVectorTest, getBoostVector)
{
	ASSERT_DOUBLE_EQ(daughter1.getBoostVector().getX(), .4);
	ASSERT_DOUBLE_EQ(daughter1.getBoostVector().getY(), .3);
	ASSERT_DOUBLE_EQ(daughter1.getBoostVector().getZ(), .2);
}

TEST_F(LorentzVectorTest, testBoosting)
{
	pxl::LorentzVector mother = pxl::LorentzVector();
	mother += daughter1;
	mother += daughter2;
	pxl::Basic3Vector vec_boost_to_CM = mother.getBoostVector();

	daughter1.boost(-vec_boost_to_CM);
	daughter2.boost(-vec_boost_to_CM);

	//calculate angle between particles, should be -1.
	double daughter1_p = sqrt(
			daughter1.getPx() * daughter1.getPx()
					+ daughter1.getPy() * daughter1.getPy()
					+ daughter1.getPz() * daughter1.getPz());
	double daughter2_p = sqrt(
			daughter2.getPx() * daughter2.getPx()
					+ daughter2.getPy() * daughter2.getPy()
					+ daughter2.getPz() * daughter2.getPz());

	double cos_angle = daughter1.getPx() * daughter2.getPx()
			+ daughter1.getPy() * daughter2.getPy()
			+ daughter1.getPz() * daughter2.getPz();
	cos_angle = cos_angle / daughter1_p;
	cos_angle = cos_angle / daughter2_p;
	ASSERT_DOUBLE_EQ(cos_angle, -1);

	//check values itself
	EXPECT_NEAR(daughter1.getPx(), 0.224832241127, 0.000000000001);
	EXPECT_NEAR(daughter1.getPy(), 0.168624180845, 0.000000000001);
	EXPECT_NEAR(daughter1.getPz(), 0.112416120564, 0.000000000001);
	EXPECT_NEAR(daughter1.getE(), 842.615031685, 0.000000001);
}

TEST_F(LorentzVectorTest, getRapidity)
{
	EXPECT_NEAR(daughter1.getRapidity(), 0.602063782392, 0.000000000001);
}

TEST_F(LorentzVectorTest, getBeamlineRapidity)
{
	EXPECT_NEAR(daughter1.getBeamlineRapidity(), 0.202732554054, 0.000000000001);
}

TEST_F(LorentzVectorTest, deltaRapidity)
{
	EXPECT_NEAR(daughter1.deltaRapidity(daughter2), 0.000682444005, 0.000000000001);
}

TEST_F(LorentzVectorTest, deltaBeamlineRapidity)
{
	EXPECT_NEAR(daughter1.deltaBeamlineRapidity(daughter2), 0.0001875117217, 0.000000000001);
}

class BasicContainerTest: public testing::Test
{
protected:
	pxl::BasicContainer *bc;
	pxl::BasicNVector *v1;
	pxl::BasicNVector *v2;

	virtual void SetUp()
	{
		bc = new pxl::BasicContainer();
		v1 = new pxl::BasicNVector();
		bc->insertObject(v1);
		v2 = new pxl::BasicNVector();
	}

	virtual void TearDown()
	{
		delete bc;
		delete v2;
	}
};

TEST_F(BasicContainerTest, has)
{
	EXPECT_TRUE(bc->has(v1));
	EXPECT_FALSE(bc->has(v2));
}

TEST_F(BasicContainerTest, remove)
{
	bc->remove(v1);
	ASSERT_FALSE(bc->has(v1));
}

TEST_F(BasicContainerTest, removeNonExistingObject)
{
	EXPECT_THROW(bc->remove(v2), std::runtime_error);
}

TEST_F(BasicContainerTest, take)
{
	bc->take(v1);
	ASSERT_FALSE(bc->has(v1));
	delete v1;
}

TEST_F(BasicContainerTest, clearAndSize)
{
	EXPECT_TRUE(bc->size() == 1);
	bc->clearContainer();
	EXPECT_TRUE(bc->size() == 0);
}

TEST_F(BasicContainerTest, SerializeDeserialize)
{
	pxl::Core::initialize();
	pxl::BasicContainer* bc2 = dynamic_cast<pxl::BasicContainer*>(ioTest(bc));
	std::vector<pxl::BasicNVector*> vs;
	bc2->getObjectsOfType<pxl::BasicNVector>(vs);
	ASSERT_TRUE(*(vs[0]) == (*v1));

	delete bc2;
}

TEST_F(BasicContainerTest, RemoveObjectsOfType)
{
	pxl::BasicNVector *v3 = new pxl::BasicNVector();
	bc->insertObject(v3);
	pxl::BasicMatrix *m = new pxl::BasicMatrix();
	bc->insertObject(m);

	std::vector<pxl::BasicNVector*> vs;
	bc->getObjectsOfType<pxl::BasicNVector>(vs);
	ASSERT_EQ(2, vs.size());

	size_t n = bc->removeObjectsOfType<pxl::BasicNVector>();
	ASSERT_EQ(2, n);
	std::vector<pxl::BasicNVector*> vs2;
	bc->getObjectsOfType<pxl::BasicNVector>(vs2);
	ASSERT_EQ(0, vs2.size());

	std::vector<pxl::BasicMatrix*> ms;
	bc->getObjectsOfType<pxl::BasicMatrix>(ms);
	ASSERT_EQ(ms.size(), 1);

	ASSERT_EQ(bc->removeObjectsOfType(m->getTypeId()), 1);
}

class RandomTest: public testing::Test
{
protected:
	pxl::Random *_rand;
	virtual void SetUp()
	{
		_rand = new pxl::Random();
	}

	virtual void TearDown()
	{
		delete _rand;
	}
};

TEST_F(RandomTest, RandUnitVectorOnSphere)
{
	pxl::Basic3Vector v = _rand->randUnitVectorOnSphere();
	EXPECT_TRUE(v.isUnitVector());
}

TEST_F(RandomTest, PowerLaw)
{
	double E = _rand->randPowerLaw(-0.2, 1., 10.);
	EXPECT_LE(E, 10.);
	EXPECT_GE(E, 1.0);
}

TEST_F(RandomTest, PowerLawMinusOne)
{
	double E = _rand->randPowerLaw(-1, 1., 10.);
	EXPECT_LE(E, 10.);
	EXPECT_GE(E, 1.0);
}

TEST_F(RandomTest, BrokenPowerLaw)
{
	double E = _rand->randBrokenPowerLaw(-1.68, -4.2, 25.7, 5, 200);
	EXPECT_LE(E, 200);
	EXPECT_GE(E, 5);
}

TEST_F(RandomTest, BrokenPowerLawMinusOne)
{
	double E;
	E = _rand->randBrokenPowerLaw(-1., -4.2, 25.7, 5, 200);
	EXPECT_LE(E, 200);
	EXPECT_GE(E, 5);
	E = _rand->randBrokenPowerLaw(-1.68, -1, 25.7, 5, 200);
	EXPECT_LE(E, 200);
	EXPECT_GE(E, 5);
}

TEST(InputFile, openNonExistingFile)
{
	pxl::InputFile infile;
	ASSERT_THROW(infile.open("this/file/does/not/exists"), std::runtime_error);
	ASSERT_TRUE(infile.bad());
	ASSERT_FALSE(infile.good());
}

TEST(Configuration, getSetData)
{
	pxl::Configuration c;
	c.addItem("Section1", "Key1", "TestValue");
	c.addItem("Section1", "Key2", 42);

	EXPECT_EQ(c.getItem("Section1", "Key1").toString(), "TestValue");
	EXPECT_EQ(c.getItem("Section1", "Key2").toInt32(), 42);
}

TEST(Configuration, IterateMultipleItems)
{
	pxl::Configuration c;

	c.addItem("Section", "Key1", 1);
	c.addItem("Section", "Key1", 2);
	c.addItem("Section", "Key1", 3);
	pxl::Configuration::multimapType m = c.getSection("Section");
	std::pair<pxl::Configuration::multimapIterator,
			pxl::Configuration::multimapIterator> values = m.equal_range(
			"Key1");
	int32_t n = 0;

	for (pxl::Configuration::multimapIterator iter = values.first;
			iter != values.second; ++iter)
	{
		n += (*iter).second.asInt32();
	}
	EXPECT_EQ(n, 6);
}

class TestMessageHandler: public pxl::MessageHandler
{
public:
	size_t idA, idB;
	pxl::Variant a, b;
	TestMessageHandler() :
			idA(0), idB(0)
	{
		pxl::MessageDispatcher &md = pxl::MessageDispatcher::instance();
		idA = md.subscribe("TestMessageIdA", this);
		idB = md.subscribe("TestMessageIdB", this);
	}

	void handleMessage(size_t mid, const pxl::Variant &parameter)
	{
		if (mid == idA)
		{
			a = parameter;
		}
		else if (mid == idB)
		{
			b = parameter;
		}
	}
};

TEST(MessageDispatcher, Test)
{
	pxl::Variant a("asda"), b(123);
	pxl::MessageDispatcher &md = pxl::MessageDispatcher::instance();
	{
		TestMessageHandler handler;
		EXPECT_NE(handler.a, a);
		EXPECT_NE(handler.b, b);

		md.publish("TestMessageIdA", a);
		EXPECT_EQ(handler.a, a);
		EXPECT_NE(handler.b, a);

		md.publish("TestMessageIdB", b);
		EXPECT_EQ(handler.b, b);
		EXPECT_EQ(handler.a, a);
	}

	// check if handler unsubscibed
	md.publish("TestMessageIdA", a);
	md.publish("TestMessageIdA", b);

}


TEST(UserRecordHelper, TestEraseOfNonExistingElement)
{
  pxl::UserRecordHelper ur;
  EXPECT_THROW(ur.eraseUserRecord("foo"), std::runtime_error);
  ur.setUserRecord("foo", 1.0);
  ur.eraseUserRecord("foo");
}



//
// Run the test cases on the serealizables in core 
//
typedef Types<pxl::BasicNVector, pxl::BasicContainer, pxl::BasicMatrix,
		pxl::Event, pxl::InformationChunk, pxl::Object> SerealizableCoreImplementations;
INSTANTIATE_TYPED_TEST_CASE_P(SerealizablesInCore, // Instance name
		SerializableTest,// Test case name
		SerealizableCoreImplementations);

