#include "gtest/gtest.h"
#include "pxl/modules.hh"
#include "pxl/scripting.hh"
#include "pxl/core/logging.hh"
#include "pxl/core/Variant.hh"

#include "pxl/core/NamedTMPFile.hh"
#include "test_share.hh"
static pxl::Logger logger("pxl::test_module");

// our module inherits from pxl::Module
class TestModule: public pxl::Module
{
private:
	uint32_t _counter;
public:

	// initialize the super class Module as well
	TestModule() :
			Module(), _counter(0)
	{
	}
	void initialize()
	{
		addSource("output", "Output");
		addSink("input", "Input");
		addOption("stringOption", "stringOptionDescription", "foo");
		addOption("int64Option", "int64OptionDescription", int64_t(42));
		addOption("boolOption", "boolOptionDescription", true);
		addOption("doubleOption", "doubleOptionDescription", 1.2);
		_counter = 0;
	}

	~TestModule()
	{
	}

	uint32_t getCounter()
	{
		return _counter;
	}

	// every Module needs a unique type
	static const std::string &getStaticType()
	{
		static std::string type("TestModule");
		return type;
	}

	// static and dynamic methods are needed
	const std::string &getType() const
	{
		return getStaticType();
	}

	bool isRunnable() const
	{
		// this module does not proide events, so return false
		return false;
	}
	void beginJob()
	{
	}

	bool analyse(pxl::Sink *sink)
	{
		_counter++;
		getSource("output")->setTargets(getSink("input")->get());
		return getSource("output")->processTargets();
	}

	void destroy()
	{
		// only we know how to delete this module
		delete this;
	}
};

class TestGeneratorModule: public pxl::Module
{
private:
	int64_t _numberOfObjects;
	uint32_t _counter;
	pxl::Serializable* _Object;
public:

	// initialize the super class Module as well
	TestGeneratorModule() :
			Module(), _numberOfObjects(0), _counter(0), _Object(0)
	{
	}
	void initialize()
	{
		addSource("output", "Output");
		addOption("NumberOfObjects", "int64OptionDescription", int64_t(5));
	}

	~TestGeneratorModule()
	{
	}

	// every Module needs a unique type
	static const std::string &getStaticType()
	{
		static std::string type("TestGeneratorModule");
		return type;
	}

	// static and dynamic methods are needed
	const std::string &getType() const
	{
		return getStaticType();
	}

	bool isRunnable() const
	{
		// this module does not proide events, so return false
		return true;
	}
	void beginJob()
	{
		getOption("NumberOfObjects", _numberOfObjects);
	}

	bool analyse(pxl::Sink *sink)
	{
		_counter++;
		if (_Object != 0)
		{
			delete _Object;
			_Object = 0;
		}
		if (_counter <= _numberOfObjects)
		{
			logger(pxl::LOG_LEVEL_DEBUG, "TestGeneratorModule Object", _counter,
					"/", _numberOfObjects);
			_Object = new pxl::BasicContainer();
			getSource("output")->setTargets(_Object);
			return getSource("output")->processTargets();
		}
		else
		{
			return false;
		}
	}

	void destroy()
	{
		// only we know how to delete this module
		if (_Object)
			delete _Object;
		delete this;
	}
};

static pxl::ModuleProducerTemplate<TestModule> _TestModuleProducer;
static pxl::ModuleProducerTemplate<TestGeneratorModule> _TestgeneratorModuleProducer;

TEST(ModuleFactory, ReUnregisterModuleProducerInterface)
{
	pxl::ModuleFactory::instance().registerModuleProducer(&_TestModuleProducer,
			std::string("TESTMODULE"));
	pxl::Module* m = pxl::ModuleFactory::instance().createModule("TESTMODULE");
	EXPECT_FALSE(m==NULL);
	m->destroy();

	pxl::ModuleFactory::instance().unregisterModuleProducer(
			&_TestModuleProducer);

	m = pxl::ModuleFactory::instance().createModule("TESTMODULE");
	EXPECT_TRUE(m==NULL);
}

TEST(ModuleFactory, ConstructDefaultModules)
{
	pxl::Modules::initialize();
	pxl::ModuleFactory mf = pxl::ModuleFactory::instance();
	for (std::vector<std::string>::const_iterator iter =
			mf.getAvailableModules().begin();
			iter != mf.getAvailableModules().end(); ++iter)
	{
		pxl::Module *m = NULL;
		m = mf.createModule((*iter));
		EXPECT_FALSE(m == NULL);
		if (m != NULL)
		{
			m->destroy();
		}
	}
}

class TestModuleTest: public testing::Test
{
protected:
	pxl::Module *m;

	virtual void SetUp()
	{
		m = new TestModule();
		m->initialize();
	}

	virtual void TearDown()
	{
		m->destroy();
	}
};

TEST_F(TestModuleTest, addOptionFirst)
{
// the option has to be added first before we can set the option
	EXPECT_THROW(m->setOption("foo", 2.0), std::runtime_error);
	m->addOption("foo", "fooOpt", 2.0);
	EXPECT_NO_THROW(m->setOption("foo", 2.0));
}

TEST_F(TestModuleTest, hasOptions)
{
	EXPECT_TRUE(m->hasOption("stringOption"));
	EXPECT_TRUE(m->hasOption("int64Option"));
	EXPECT_TRUE(m->hasOption("boolOption"));
	EXPECT_TRUE(m->hasOption("doubleOption"));
	EXPECT_FALSE(m->hasOption("fooOption"));
}

TEST_F(TestModuleTest, hasSink)
{
	EXPECT_TRUE(m->hasSink("input"));
	EXPECT_FALSE(m->hasSink("output"));
}

TEST_F(TestModuleTest, hasSource)
{
	EXPECT_TRUE(m->hasSource("output"));
	EXPECT_FALSE(m->hasSource("input"));
}

TEST_F(TestModuleTest, getSink)
{
	EXPECT_TRUE(m->getSink("input")!=NULL);
	EXPECT_THROW(m->getSink("output"), std::runtime_error);
}

TEST_F(TestModuleTest, getSource)
{
	EXPECT_TRUE(m->getSource("output")!=NULL);
	EXPECT_THROW(m->getSource("input"), std::runtime_error);
}

TEST_F(TestModuleTest, setOption)
{
	double d = 1.0;
	m->setOption("doubleOption", 4.2);
	m->getOption("doubleOption", d);
	EXPECT_DOUBLE_EQ(4.2, d);
}


class AnalysisTest: public testing::Test
{
protected:
	pxl::Analysis *A;
	virtual void SetUp()
	{
		A = new pxl::Analysis();
		pxl::Module *m = new TestModule();
		m->setName("TestModule");
		m->initialize();
		A->addModule(m);
		_TestgeneratorModuleProducer.initialize();
		A->addModule("TestGeneratorModule", "Generator", NULL);
		m = A->getModule("Generator");
		m->initialize();
		A->connectModules("Generator", "output", "TestModule", "input");
	}
	virtual void TearDown()
	{
		delete A;
	}
};

TEST_F(AnalysisTest, connectionRemovalOnModuleRemoval)
{
	// test whether all connections are correctly removed if the module are
	// removed.
	EXPECT_EQ(A->getConnections().size(), 1);

	// Adding an additional generator module and connect it to the
	// testmodule
	A->addModule("TestGeneratorModule", "Generator2", NULL);
	A->connectModules("Generator2", "output", "TestModule", "input");
	EXPECT_EQ(A->getConnections().size(), 2);

	//after removing the TestModule there should be no connection
	A->removeModule("TestModule");
	EXPECT_EQ(A->getConnections().size(), 0);
}

TEST_F(AnalysisTest, addStringVectorOption)
{
	pxl::Module *m1 = A->addModule("File Input", "TM1", "0");
	pxl::Module *m2 = A->addModule("File Input", "TM2", "0");
	A->setModuleOption("TM1", "File names", "('A','B')");
	A->setModuleOption("TM2", "File names", "['C','D']");
	std::vector<std::string> opts;
	m1->getOption("File names", opts);
	EXPECT_EQ(opts.size(), 2);
	EXPECT_EQ(opts[0], "A");
	EXPECT_EQ(opts[1], "B");

	m2->getOption("File names", opts);
	EXPECT_EQ(opts.size(), 2);
	EXPECT_EQ(opts[0], "C");
	EXPECT_EQ(opts[1], "D");
}

TEST_F(AnalysisTest, findFile)
{
	//Test the find file method of the analysis class
	EXPECT_TRUE(A->findFile("this_file_does_not_exist") == "");
	EXPECT_TRUE(A->findFile("") == "");
}

TEST_F(AnalysisTest, removeModule)
{
	EXPECT_TRUE(A->hasModule("TestModule"));
	A->removeModule("TestModule");
	EXPECT_FALSE(A->hasModule("TestModule"));
	EXPECT_TRUE(A->hasModule("Generator"));
	EXPECT_THROW(A->removeModule("TestModule"), std::runtime_error);

}

TEST_F(AnalysisTest, getModule)
{
	EXPECT_TRUE(A->getModule("TestModule")!=NULL);
	EXPECT_THROW(A->getModule("Foo"), std::runtime_error);
}

TEST_F(AnalysisTest, disconnectModules)
{
	EXPECT_THROW(A->disconnectModules("Foo", "output", "TestModule", "input"),
			std::runtime_error);
	EXPECT_THROW(A->disconnectModules("Generator", "output", "bar", "input"),
			std::runtime_error);
	EXPECT_THROW(
			A->disconnectModules("Generator", "foo", "TestModule", "input"),
			std::runtime_error);
	EXPECT_THROW(
			A->disconnectModules("Generator", "output", "TestModule", "bar"),
			std::runtime_error);
	A->disconnectModules("Generator", "output", "TestModule", "input");
	EXPECT_THROW(
			A->disconnectModules("Generator", "output", "TestModule", "input"),
			std::runtime_error);
}

TEST_F(AnalysisTest, AnalysisExecution)
{
	A->setModuleOption("Generator", "NumberOfObjects", "3");
	A->run();
	TestModule *m = dynamic_cast<TestModule*>(A->getModule("TestModule"));
	EXPECT_EQ(3, m->getCounter());
}

TEST_F(AnalysisTest, AnalysisExecutionDisabledModule)
{
	A->setModuleOption("Generator", "NumberOfObjects", "3");
	TestModule *m = dynamic_cast<TestModule*>(A->getModule("TestModule"));
	m->setEnabled(false);
	A->run();
	EXPECT_EQ(0, m->getCounter());
}

// Test behaviour of python modules
//
class PyAnalyseModuleTest: public testing::Test
{
protected:
	pxl::Analysis *A;
	pxl::PyAnalyseModule *module;
	virtual void SetUp()
	{
		pxl::Script::addModulePath(LIBRARY_OUTPUT_PATH);
		A = new pxl::Analysis();
		_TestgeneratorModuleProducer.initialize();
		A->addModule("TestGeneratorModule", "Generator", NULL);
		module = new pxl::PyAnalyseModule();
		module->initialize();
		module->setName("PyAnalyseModule");
		//initialize modules to generate defautl sink and source
		A->addModule(module);
		A->connectModules("Generator", "output", "PyAnalyseModule", "in");
	}

	virtual void TearDown()
	{
		delete A;
	}

};

TEST_F(PyAnalyseModuleTest, testEmptyScriptFileName)
{
	// the analyse module holds no script, analysis execution should
	// throw a runtime error 
	EXPECT_THROW(A->run(), std::runtime_error);
}

TEST_F(PyAnalyseModuleTest, testNonExistingScriptFile)
{
	// a non existing file should also generate an error
	module->setOption("filename", "this_file_does_not_exist");
	EXPECT_THROW(A->run(), std::runtime_error);
}

// How does the script option work??
//TEST_F(PyAnalyseModuleTest, testScriptOption)
//{
//	// a non existing file should also generate an error
//	module->setOption("script","print 'This works' ");
//	EXPECT_NO_THROW(A->run());
//}

TEST_F(PyAnalyseModuleTest, testPythonCodeExecution)
{
	std::string t = TESTSOURCEDIR;
	logger(pxl::LOG_LEVEL_DEBUG, "TESTSOURCEDIR", t);
	t += "/count_script.py";
	logger(pxl::LOG_LEVEL_DEBUG, "script name", t);
	module->setOption("filename", t);
	EXPECT_NO_THROW(A->run());
}

TEST_F(PyAnalyseModuleTest, testImportErrorInModule)
{
	//creates a buggy script by inserting a non existing import to
	//tmpfile
	pxl::NamedTMPFile tmpFileName;
	FILE *tmpFilePointer = fopen(tmpFileName.getName().c_str(), "w");
	fprintf(tmpFilePointer, "import non_existing_module\n");

	std::string t = TESTSOURCEDIR;
	t += "/count_script.py";
	FILE *inFile = fopen(t.c_str(), "r");
	char buffer[80];
	while (fgets(buffer, 80, inFile) != NULL)
	{
		fputs(buffer, tmpFilePointer);
	}
	fclose(tmpFilePointer);
	fclose(inFile);
	//set option filename triggers a reload, which will throw an exception
	EXPECT_THROW(module->setOption("filename", tmpFileName.getName().c_str()),
			std::runtime_error);
	EXPECT_THROW(A->run(), std::runtime_error);
}

TEST_F(PyAnalyseModuleTest, testFixOfErrorInModule)
{
	//creates a buggy script by inserting a non existing import to
	//tmpfile
	//this is later removed and reloaded - the error message of the module
	//should be cleared
	pxl::NamedTMPFile tmpFileName;
	FILE *tmpFilePointer = fopen(tmpFileName.getName().c_str(), "w");
	fprintf(tmpFilePointer, "import non_existing_module\n");

	std::string t = TESTSOURCEDIR;
	t += "/count_script.py";
	FILE *inFile = fopen(t.c_str(), "r");
	char buffer[80];
	while (fgets(buffer, 80, inFile) != NULL)
	{
		fputs(buffer, tmpFilePointer);
	}
	fclose(tmpFilePointer);
	fclose(inFile);
	//set option filename triggers a reload, which will throw an exception
	EXPECT_THROW(module->setOption("filename", tmpFileName.getName().c_str()),
			std::runtime_error);
	EXPECT_THROW(A->run(), std::runtime_error);

	//now fix the error
	tmpFilePointer = fopen(tmpFileName.getName().c_str(), "w");
	inFile = fopen(t.c_str(), "r");
	while (fgets(buffer, 80, inFile) != NULL)
	{
		fputs(buffer, tmpFilePointer);
	}
	fclose(tmpFilePointer);
	fclose(inFile);

	// this is a now working script
	EXPECT_NO_THROW(module->reload());

	EXPECT_EQ(module->getStatusMessages().size(), 0);
}
