// Helper classes for testing
#include "gtest/gtest.h"
#include <pxl/core.hh>
#include <stdlib.h>


// Writes object to temporary file and reads it again.
pxl::Serializable* ioTest(const pxl::Serializable* inputObject)
{

	pxl::NamedTMPFile tmpFile;

	pxl::OutputFile ofile(tmpFile.getName());
	ofile.streamObject(inputObject);
	ofile.close();

	pxl::InputFile inFile(tmpFile.getName());
	pxl::Serializable* obj = inFile.readNextObject();
	inFile.close();
	EXPECT_TRUE(obj != 0);
	if (obj)
	{
		EXPECT_TRUE(inputObject->getId() == obj->getId());
	}
	return obj;
}
