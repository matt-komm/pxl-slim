#
# create swigpyrun
#
IF (USE_SWIG)
    ADD_CUSTOM_COMMAND(
    	OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/swigpyrun.h
    	COMMAND ${SWIG_EXECUTABLE} -python -c++ -classic -external-runtime swigpyrun.h
    )
ENDIF(USE_SWIG)

#
# setup include directories
#
include_directories (
	include
	${PXL_SOURCE_DIR}/hep/include
	${PXL_SOURCE_DIR}/core/include
	${CMAKE_CURRENT_BINARY_DIR}
	${PYTHON_INCLUDE_PATH}
	${CMAKE_BINARY_DIR}/include
)


add_definitions(-DPYTHON_VERSION=${PYTHON_VERSION})


#
# add library using the sources
#
add_library(pxl-scripting SHARED
	src/Script.cc
	src/DefaultScriptFilter.cc
	src/EventViewScriptFilter.cc
	src/ParticleScriptFilter.cc
	swigpyrun.h
)

#
# scripting depends on pxl and hep
#
add_dependencies(pxl-scripting pxl-core pxl-hep)

#
# link to pxl
#
target_link_libraries(pxl-scripting pxl-core pxl-hep "${PYTHON_LIBRARIES}" ${CMAKE_THREAD_LIBS_INIT})
if (UNIX)
	target_link_libraries(pxl-scripting util)
endif(UNIX)

#
# setup install path
#
PXL_INSTALL_LIBRARY(pxl-scripting) 

#
# set library properties for Mac OS X
#
SET_XCODE_FRAMEWORK_PROPERTIES(pxl-scripting)

