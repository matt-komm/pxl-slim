#
# use local include directory
#
include_directories (
	include
	${PXL_SOURCE_DIR}/scripting/include
	${PXL_SOURCE_DIR}/hep/include
	${PXL_SOURCE_DIR}/core/include
	${CMAKE_BINARY_DIR}/include
)

#
# add library using the sources
#
add_library(pxl-algorithms SHARED
    src/AutoLayout.cc 
)

#
# hep depends on pxl, hep and scripting
#
add_dependencies(pxl-algorithms pxl-core pxl-hep pxl-scripting) 

#
# link to pxl, hep and scripting
#
target_link_libraries (pxl-algorithms pxl-core pxl-hep pxl-scripting)

#
# setup install path
#
PXL_INSTALL_LIBRARY(pxl-algorithms)

#
# set library properties for Mac OS X
#
SET_XCODE_FRAMEWORK_PROPERTIES(pxl-algorithms)