#include "gtest/gtest.h"
#include "pxl/scripting.hh"

class TestScript : public testing::Test
{
	protected:
		pxl::Script *script;
		virtual void SetUp()
		{
			script = new pxl::Script();
			pxl::Script::addModulePath(LIBRARY_OUTPUT_PATH);
		}
		virtual void TearDown()
		{
			delete script;
		}
};

TEST_F(TestScript, testWorkingScript)
{
	EXPECT_TRUE(script->create("sys.stdout.write ('foo')"));
	EXPECT_NO_THROW(script->call("__main__",NULL));
}

TEST_F(TestScript, testBuggyScript)
{
	EXPECT_THROW(script->create("sys.stdout.write ('foo)"), std::runtime_error);
	EXPECT_FALSE(script->call("__main__",NULL));
}

TEST_F(TestScript, loadScript)
{
	std::string t = TESTSOURCEDIR;
	t+="/count_script.py";
	EXPECT_TRUE(script->load(t));

}
