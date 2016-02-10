#
# use local include directory
#
include_directories (
	${PXL_SOURCE_DIR}/modules/include
	${PXL_SOURCE_DIR}/scripting/include
	${PXL_SOURCE_DIR}/algorithms/include
	${PXL_SOURCE_DIR}/core/include
	${PXL_SOURCE_DIR}/hep/include
	${PXL_SOURCE_DIR}/astro/include
	${PXL_SOURCE_DIR}/xml/include
	${PXL_SOURCE_DIR}/lib
	${CMAKE_BINARY_DIR}/include
)


#
# add executable using the sources
#
add_executable(pxlrun 
	src/main.cc
)

#
# setup installation information
#
add_definitions(-DPXL_VERSION="${PXLVERSION}")
add_definitions(-DPXL_DATA_PATH="${datadir}")

add_definitions(-DPXL_PREFIX="${CMAKE_INSTALL_PREFIX}")
add_definitions(-DPXL_INCDIR="${includedir}")
add_definitions(-DPXL_LIBDIR="${libdir}")

set(PXL_SYSTEM_RC "/etc/pxlrc" CACHE STRING "system wide pxlrc" )
add_definitions(-DPXL_SYSTEM_RC="${PXL_SYSTEM_RC}")


#
# setup dependencies
#
add_dependencies(pxlrun pxl-core pxl-xml pxl-modules)


#
# link to pxl
#
target_link_libraries (pxlrun pxl-core pxl-hep pxl-astro pxl-modules pxl-xml)


#
# setup install path
#
PXL_INSTALL_EXECUTABLE(pxlrun)


