#include "pxl/core/Serializable.hh"
//
// Typed Test for serializable core functions
//
// http://code.google.com/p/googletest/source/browse/trunk/samples/sample6_unittest.cc
using testing::Types;

// test fixture class tempalte
template <class T>
class SerializableTest: public testing::Test {
 protected:
	// The ctor calls the factory function to create a prime table
	// implemented by T.
	SerializableTest() { _serializable = new T();}

	virtual ~SerializableTest() { delete _serializable; }

	virtual bool testTypeId()
	{
		return (_serializable->getTypeId() == T::getStaticTypeId());
	}

	// Note that we test an implementation via the base interface
	// instead of the actual implementation class.  This is important
	// for keeping the tests close to the real world scenario, where the
	// implementation is invoked via the base interface.  It avoids
	// got-yas where the implementation class has a method that shadows
	// a method with the same name (but slightly different argument
	// types) in the base interface, for example.
	pxl::Serializable* _serializable;
};

TYPED_TEST_CASE_P(SerializableTest);

TYPED_TEST_P(SerializableTest, staticTypeIdMatchesTypeId) {
	EXPECT_TRUE(this->testTypeId());
}

REGISTER_TYPED_TEST_CASE_P(
	SerializableTest,  // The first argument is the test case name.
	// The rest of the arguments are the test names.
	staticTypeIdMatchesTypeId);

