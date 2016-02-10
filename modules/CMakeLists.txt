#
# use local include directory
#
include_directories (
    include
	${PXL_SOURCE_DIR}/core/include 
	${PXL_SOURCE_DIR}/scripting/include 
	${PXL_SOURCE_DIR}/hep/include 
	${PXL_SOURCE_DIR}/algorithms/include 
	${PXL_SOURCE_DIR}/lib
	${CMAKE_BINARY_DIR}/include
	
)

#
# add library using the sources
#
add_library (pxl-modules SHARED
	src/Analysis.cc
	src/InputModule.cc
	src/Module.cc
	src/Modules.cc
	src/ModuleFactory.cc 
	src/OutputModule.cc
	src/PyAnalyseModule.cc
	src/PyDecideModule.cc
	src/PyGeneratorModule.cc
	src/PyModuleModule.cc
	src/PyModule.cc
	src/PySwitchModule.cc
)

#
# modules depends on pxl-core pxl-hep pxl-scripting pxl-algorithms
#
add_dependencies(pxl-modules pxl-core pxl-hep pxl-scripting pxl-algorithms) 

#
# link to pxl pxl-hep pxl-scripting pxl-algorithms
#
target_link_libraries(pxl-modules pxl-core pxl-hep pxl-scripting pxl-algorithms)

PXL_INSTALL_LIBRARY(pxl-modules)

#
# set library properties for Mac OS X
#
SET_XCODE_FRAMEWORK_PROPERTIES(pxl-modules)