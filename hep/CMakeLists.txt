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
add_library (pxl-hep SHARED
    src/AnalysisFork.cc
    src/AnalysisProcess.cc 
    src/Collision.cc
    src/EventView.cc
    src/Particle.cc
    src/Vertex.cc
    src/Hep.cc
)

#
# hep depends only on pxl base and io
#
add_dependencies(pxl-hep pxl-core) 

#
# link to pxl
#
target_link_libraries (pxl-hep pxl-core) 

#
# setup install path
#
PXL_INSTALL_LIBRARY(pxl-hep)
 
#
# set library properties for Mac OS X
#
SET_XCODE_FRAMEWORK_PROPERTIES(pxl-hep)
