#
# use local include directory
#
include_directories (include ${ZLIB_INCLUDE_DIR} ${CMAKE_BINARY_DIR}/include)

set(PXL_CORE_EXTRA_FILES)
if (PXL_ENABLE_SFTP)
    list(APPEND PXL_CORE_EXTRA_FILES "src/sFTPFileImpl.cc")
    include_directories (${LIB_SSH_INCLUDE_DIR}) 
    
endif()

if (PXL_ENABLE_DCAP)
    list(APPEND PXL_CORE_EXTRA_FILES "src/dCapFileImpl.cc")
    include_directories (${LIB_DCAP_INCLUDE_DIR}) 
    
endif()

#
# add library using the sources
#
add_library (pxl-core SHARED
	src/Basic3Vector.cc
	src/BasicContainer.cc
	src/BasicMatrix.cc
	src/BasicNVector.cc
	src/Core.cc
	src/Configuration.cc
	src/ConfigLoader.cc
	src/Event.cc
	src/File.cc
	src/functions.cc
	src/Id.cc
	src/InformationChunk.cc
	src/logging.cc
	src/LorentzVector.cc
	src/MessageDispatcher.cc
	src/Object.cc
	src/ObjectManager.cc
	src/ObjectOwner.cc
	src/Random.cc
	src/Relations.cc
	src/Relative.cc
	src/SoftRelations.cc
	src/UserRecord.cc
	src/Variant.cc
	src/WkPtrBase.cc 
	src/ChunkReader.cc
	src/ChunkWriter.cc
	src/InputHandler.cc
	src/Tokenizer.cc
	src/ObjectFactory.cc
	src/OutputFile.cc
	src/OutputHandler.cc
	src/PluginManager.cc
	src/FileFactory.cc
	src/LocalFileImpl.cc
	src/NamedTMPFile.cc
	${PXL_CORE_EXTRA_FILES}
)

#
# link to pxl
#
target_link_libraries(pxl-core ${ZLIB_LIBRARIES})

add_definitions(-DPXL_PLUGIN_LIBEXEC_DIR=${PXLPLUGINDIR})

CONFIGURE_FILE(config.hh.in ${CMAKE_BINARY_DIR}/include/pxl/core/config.hh) 
INSTALL(FILES ${CMAKE_BINARY_DIR}/include/pxl/core/config.hh DESTINATION ${includedir}/pxl/core)
#
# add winmm for timer function on Windows
#
if (WIN32)
	target_link_libraries (pxl-core winmm ws2_32)
else (WIN32)
    target_link_libraries (pxl-core dl)
endif (WIN32)

if (UNIX AND NOT APPLE)
    add_definitions(-D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE)
endif()

if(PXL_ENABLE_SFTP)
    target_link_libraries (pxl-core ${LIB_SSH_LIBRARIES} )
endif()

if(PXL_ENABLE_DCAP)
    target_link_libraries (pxl-core ${LIB_DCAP_LIBRARIES} )
endif()

#
# setup install path
#
PXL_INSTALL_LIBRARY(pxl-core)

#
# set library properties for Mac OS X
#
SET_XCODE_FRAMEWORK_PROPERTIES(pxl-core)

