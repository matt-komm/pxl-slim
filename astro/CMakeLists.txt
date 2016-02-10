#
# use local include directory
#
include_directories (
    include
    ${PXL_SOURCE_DIR}/core/include
	${CMAKE_BINARY_DIR}/include
)
 
#
# add library using the sources
#
add_library (pxl-astro SHARED
	src/Astro.cc
    src/AstroBasicObject.cc
    src/AstroObject.cc
    src/RegionOfInterest.cc
    src/UHECR.cc
    src/UHECRSource.cc
    src/CoordinateTransformations.cc
)


#
# astro depends only on pxl core 
#
add_dependencies(pxl-astro pxl-core)

#
# link to pxl
#
target_link_libraries (pxl-astro pxl-core) 

#
# setup install path
#
PXL_INSTALL_LIBRARY(pxl-astro) 


#
# set library properties for Mac OS X
#
SET_XCODE_FRAMEWORK_PROPERTIES(pxl-astro)


#
# pxl-healpix includes the healpix part, which is under GPL
#
OPTION(USE_HEALPIX "Build the healpix library, which is under GPL" TRUE)

if (USE_HEALPIX)
	add_library (pxl-healpix SHARED
		src/Healpix.cc
		src/HealpixMap.cc
	)
	add_dependencies(pxl-healpix pxl-astro pxl-core)
	target_link_libraries (pxl-healpix pxl-astro pxl-core) 

	PXL_INSTALL_LIBRARY(pxl-healpix) 
	SET_XCODE_FRAMEWORK_PROPERTIES(pxl-healpix)
endif(USE_HEALPIX)



