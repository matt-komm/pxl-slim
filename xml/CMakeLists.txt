#
# use local include directory
#
include_directories (
	include
	${PXL_SOURCE_DIR}/core/include
	${PXL_SOURCE_DIR}/hep/include
	${PXL_SOURCE_DIR}/astro/include
	${PXL_SOURCE_DIR}/xml/include
	${PXL_SOURCE_DIR}/modules/include
	${PXL_SOURCE_DIR}/algorithms/include
	${PXL_SOURCE_DIR}/scripting/include
	${CMAKE_BINARY_DIR}/include
)

#
# setup expat
#
OPTION(INTERNAL_EXPAT "use internal expat" OFF)
FIND_PACKAGE(EXPAT)
IF(EXPAT_FOUND AND NOT INTERNAL_EXPAT)
	OPTION(STATIC_EXPAT "external expat is static" OFF)
	SET (EXPAT_SRCS)
	if (STATIC_EXPAT)
		add_definitions(-DXML_STATIC)
	endif(STATIC_EXPAT)
	MESSAGE(STATUS "Use system expat")
	INCLUDE_DIRECTORIES(${EXPAT_INCLUDE_DIR})
ELSE(EXPAT_FOUND  AND NOT INTERNAL_EXPAT)
	MESSAGE(STATUS "Use internal expat")
	if (WIN32)
		add_definitions(-DCOMPILED_FROM_DSP)
	endif (WIN32)
	add_definitions(-DXML_STATIC -DHAVE_MEMMOVE)
	include_directories(${CMAKE_SOURCE_DIR}/lib/expat)
	SET(EXPAT_SRCS ${CMAKE_SOURCE_DIR}/lib/expat/xmlparse.c ${CMAKE_SOURCE_DIR}/lib/expat/xmlrole.c ${CMAKE_SOURCE_DIR}/lib/expat/xmltok.c)
	SET(EXPAT_LIBRARIES)
ENDIF(EXPAT_FOUND AND NOT INTERNAL_EXPAT)

#
# add library using the sources
#
add_library (pxl-xml SHARED
	src/XmlHandler.cc 
	src/XmlStream.cc 
	src/AnalysisXmlImport.cc 
	src/AnalysisXmlWriter.cc 
	src/AnalysisXmlExport.cc 
	src/ModuleXmlHandler.cc
	src/UserRecordsXmlHandler.cc
	src/UserRecordsXmlWriter.cc
	${EXPAT_SRCS}
)

#
# xml depends only on pxl 
#
add_dependencies(pxl-xml pxl-core pxl-modules) 

#
# xml links to pxl and hep
#
target_link_libraries (pxl-xml pxl-core pxl-modules ${EXPAT_LIBRARIES})

#
# setup install path
#
PXL_INSTALL_LIBRARY(pxl-xml)

#
# set library properties for Mac OS X
#
SET_XCODE_FRAMEWORK_PROPERTIES(pxl-xml)

