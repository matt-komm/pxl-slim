#include "gtest/gtest.h"
#include "pxl/xml.hh"
#include "pxl/modules.hh"
#include "pxl/core/NamedTMPFile.hh"
#include <sstream>
#include <iostream>

//#include "test_share.hh"

using namespace pxl;

class XmlStreamTest : public testing::Test
{
	protected:
		std::stringstream sStream;
		pxl::XmlStream* xmlStream;
		virtual void SetUp()
		{
			xmlStream = new pxl::XmlStream(sStream);
		}

		virtual void TearDown()
		{
			delete xmlStream;
		}

		std::string getResult()
		{
			//flushing the stream before returning the c_str
			xmlStream->getStream().flush();
			return sStream.str();
		}
};

TEST_F(XmlStreamTest, startElementWithAttributes)
{
	xmlStream->startElementWithAttributes("foo");
	EXPECT_TRUE("<foo" == getResult());
}

TEST(AnalysisXmlIO, ImportExport)
{
	//Create Test analysis
	Analysis* analysis1 = new Analysis();
	Module *ipm = analysis1->addModule("File Input", "Input","0");
	ipm->setEnabled(true);
	ipm->setUserRecord("TestUserRecord",42.23);
	Module *opm = analysis1->addModule("File Output", "Output","0");
	opm->setEnabled(false);
	analysis1->connectModules("Input","out","Output", "in" );
	analysis1->setUserRecord("Author","TestWriter");

	NamedTMPFile tmpFile;
	//write 
	AnalysisXmlExport analysisWriter;
	analysisWriter.open(tmpFile.getName());
	analysisWriter.writeObject(analysis1);
	analysisWriter.close();

	//read
	AnalysisXmlImport analysisReader;
	Analysis* analysis2 = new Analysis();
	analysisReader.open(tmpFile.getName());
	analysisReader.parseInto(analysis2);
	analysisReader.close();

	//check
	EXPECT_TRUE(analysis2->hasUserRecord("Author"));
	std::string author = analysis2->getUserRecord("Author");
	EXPECT_STREQ("TestWriter",author.c_str());
	Module *ipm2 = analysis2->getModule("Input");
	Module *opm2 = analysis2->getModule("Output");


	EXPECT_TRUE(ipm2->hasUserRecord("TestUserRecord"));
	EXPECT_DOUBLE_EQ(ipm2->getUserRecord("TestUserRecord"),42.23 );

	EXPECT_TRUE(ipm2->isEnabled());
	EXPECT_FALSE(opm2->isEnabled());
	delete analysis1;
	delete analysis2;



}
