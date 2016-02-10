#
# use local include directory
#


INCLUDE_DIRECTORIES(
	${PXL_SOURCE_DIR}/core/include
	${PXL_SOURCE_DIR}/hep/include
	${PXL_SOURCE_DIR}/astro/include
	${PXL_SOURCE_DIR}/modules/include
	${PXL_SOURCE_DIR}/algorithms/include
	${PXL_SOURCE_DIR}/scripting/include
	${PXL_SOURCE_DIR}/xml/include
	${PYTHON_INCLUDE_PATH}
	${CMAKE_BINARY_DIR}/include	
)


MESSAGE(STATUS "Use SWIG to generate Python interfaces")

#PXL requires swig 2.0
IF(SWIG_version_output VERSION_LESS 2.0)
	MESSAGE(SEND_ERROR " PXL requires SWIG 2.0")
ENDIF()

INCLUDE(${SWIG_USE_FILE})

IF(SWIG_version_output VERSION_LESS 1.3.30)
    SET(SWIG_SELF "self")
ELSE()
    SET(SWIG_SELF "$self")
ENDIF()

SET(SWIG_FLAGS "-Wall" "-fvirtual")

#IF(SWIG_version_output VERSION_GREATER 2.0.3)
#    MESSAGE(STATUS "Use SWIG -builtin feature")
#    LIST(APPEND SWIG_FLAGS "-builtin")
#ENDIF()

IF (ENABLESWIGDOCSTRINGS)
	LIST(APPEND SWIG_FLAGS "-features" "docstring,autodoc=1")
ENDIF(ENABLESWIGDOCSTRINGS)

IF (${PYTHON_MAJOR_VERSION} STREQUAL "3")
	LIST(APPEND SWIG_FLAGS "-py3")
ENDIF()

MESSAGE(STATUS "SWIG Flags: ${SWIG_FLAGS}")

#
# Check for numpy
#

OPTION(ENABLE_NUMPY "Enables numpy interface" ON)
if (ENABLE_NUMPY)
# tries to import numpy
	execute_process(COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/checkNumpy.py" OUTPUT_VARIABLE numpyIncludePath)
	if(numpyIncludePath)
	  MESSAGE(STATUS "Found numpy headers in " ${numpyIncludePath})
	  SET(CMAKE_SWIG_FLAGS -DWITHNUMPY)
	  add_definitions(-DWITHNUMPY)
	  include_directories(${numpyIncludePath})
	else()
	  MESSAGE(STATUS "NOT FOUND: numpy - not generating numpy interface")
	endif()
ENDIF()


#
# performs all steps to add a pxl swig target
#
MACRO(PXL_ADD_SWIG_TARGET name module)
	MESSAGE( STATUS "Adding Swig Module: " ${name})
	
	SET(SRC "${CMAKE_CURRENT_BINARY_DIR}/${name}.i")
	CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/src/${name}.i.in ${SRC})
	
	# make sure C++ is used 
	SET_SOURCE_FILES_PROPERTIES(${SRC} PROPERTIES CPLUSPLUS ON)
	#SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/src/${name}.i
	#		PROPERTIES OBJECT_DEPENDS "${ARGN}")

	#Reswig dependencies 
	SET(SWIG_MODULE_${name}_EXTRA_DEPS ${ARGN})
	
	# Turn on all warnings from swig
#	SET_SOURCE_FILES_PROPERTIES(src/${name}.i PROPERTIES SWIG_FLAGS ${SWIG_FLAGS})
	SET_PROPERTY(SOURCE ${SRC} APPEND PROPERTY SWIG_FLAGS ${SWIG_FLAGS})

	# create the target directory
	file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/src)
	
	SET(CMAKE_SWIG_OUTDIR ${OUTPUT_PATH}/pxl)
	
	# create the target
	SWIG_ADD_MODULE(${name} python ${SRC} ${ARGN})
	
	# install the python and the library files
	INSTALL(FILES "${CMAKE_BINARY_DIR}/bin/pxl/${name}.py" DESTINATION ${PYTHON_SITE_PACKAGES}/pxl)
	INSTALL(TARGETS ${SWIG_MODULE_${name}_REAL_NAME} DESTINATION ${PYTHON_SITE_PACKAGES}/pxl)
	
	# link to the needed libraries
	TARGET_LINK_LIBRARIES(${SWIG_MODULE_${name}_REAL_NAME} ${PYTHON_LIBRARIES})
	TARGET_LINK_LIBRARIES(${SWIG_MODULE_${name}_REAL_NAME} ${module})
	# make sure all dependencies are build
	ADD_DEPENDENCIES(${SWIG_MODULE_${name}_REAL_NAME} ${module} pypxl-doxy)

	# on windows python modules need the .pyd extension
	IF(WIN32 AND NOT CYGWIN)
		SET_TARGET_PROPERTIES(${SWIG_MODULE_${name}_REAL_NAME} PROPERTIES SUFFIX ".pyd")
	ENDIF(WIN32 AND NOT CYGWIN)

  # install swig.i file
  INSTALL(FILES ${SRC} DESTINATION ${datadir}/swig)
ENDMACRO(PXL_ADD_SWIG_TARGET)


# Add all headers as dependencies for swig 
FILE(GLOB coreIncludes "${CMAKE_SOURCE_DIR}/core/include/pxl/core/*.hh")
PXL_ADD_SWIG_TARGET(core pxl-core "${coreIncludes}")
FILE(GLOB astroIncludes "${CMAKE_SOURCE_DIR}/astro/include/pxl/astro/*.hh")
PXL_ADD_SWIG_TARGET(astro pxl-astro
	${CMAKE_SOURCE_DIR}/astro/include/pxl/astro.hh ${astroIncludes})
FILE(GLOB hepIncludes "${CMAKE_SOURCE_DIR}/hep/include/pxl/hep/*.hh")
PXL_ADD_SWIG_TARGET(hep pxl-hep
	${CMAKE_SOURCE_DIR}/hep/include/pxl/hep.hh ${hepIncludes})
FILE(GLOB algorithmsIncludes "${CMAKE_SOURCE_DIR}/algorithms/include/pxl/algorithms/*.hh")
PXL_ADD_SWIG_TARGET(algorithms pxl-algorithms
	${CMAKE_SOURCE_DIR}/algorithms/include/pxl/algorithms.hh
	${algorithmsIncludes})
FILE(GLOB modulesIncludes "${CMAKE_SOURCE_DIR}/modules/include/pxl/modules/*.hh")
PXL_ADD_SWIG_TARGET(modules pxl-modules
	${CMAKE_SOURCE_DIR}/modules/include/pxl/modules.hh ${modulesIncludes})
FILE(GLOB xmlIncludes "${CMAKE_SOURCE_DIR}/xml/include/pxl/xml/*.hh")
PXL_ADD_SWIG_TARGET(xml pxl-xml
	${CMAKE_SOURCE_DIR}/xml/include/pxl/xml.hh ${xmlIncludes})


IF(${PYTHON_MAJOR_VERSION} STREQUAL "3")
    CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/src/__init__.py.in ${OUTPUT_PATH}/pxl/__init__.py)
ELSE()    
    CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/src/__init__.py.in ${OUTPUT_PATH}/pxl/__init__.py)
ENDIF()

INSTALL(FILES "${OUTPUT_PATH}/pxl/__init__.py" DESTINATION ${PYTHON_SITE_PACKAGES}/pxl)

ADD_TEST(PyPXLModuleLoading_core ${PYTHON_EXECUTABLE} "-c" "import sys; sys.path.insert(0,'${OUTPUT_PATH}'); sys.path.insert(0,'${LIBRARY_OUTPUT_PATH}');import pxl.core; b = pxl.core.Basic3Vector()")
ADD_TEST(PyPXLModuleLoading_hep ${PYTHON_EXECUTABLE} "-c" "import sys; sys.path.insert(0,'${OUTPUT_PATH}'); sys.path.insert(0,'${LIBRARY_OUTPUT_PATH}');import pxl.hep")
ADD_TEST(PyPXLModuleLoading_astro ${PYTHON_EXECUTABLE} "-c" "import sys; sys.path.insert(0,'${OUTPUT_PATH}'); sys.path.insert(0,'${LIBRARY_OUTPUT_PATH}');import pxl.astro")


if (USE_HEALPIX)
  PXL_ADD_SWIG_TARGET(healpix pxl-healpix
		${CMAKE_SOURCE_DIR}/astro/include/pxl/healpix.hh ${astroIncludes})
  ADD_TEST(PyPXLModuleLoading_healpix ${PYTHON_EXECUTABLE} "-c" "import sys; sys.path.insert(0,'${OUTPUT_PATH}'); sys.path.insert(0,'${LIBRARY_OUTPUT_PATH}');import pxl.healpix")
endif (USE_HEALPIX)

#ADD_TEST(PyPXL_plugins ${PYTHON_EXECUTABLE} "-c" "import sys; sys.path.insert(0,'${CMAKE_CURRENT_BINARY_DIR}'); sys.path.insert(0,'${CMAKE_SOURCE_DIR}/bin');import pxl.plugins")
ADD_TEST(PyPXLModuleLoading_modules ${PYTHON_EXECUTABLE} "-c" "import sys; sys.path.insert(0,'${OUTPUT_PATH}'); sys.path.insert(0,'${LIBRARY_OUTPUT_PATH}');import pxl.modules")
ADD_TEST(PyPXLModuleLoading_Random ${PYTHON_EXECUTABLE} "-c" "import sys; sys.path.insert(0,'${OUTPUT_PATH}'); sys.path.insert(0,'${LIBRARY_OUTPUT_PATH}');import pxl.core; r = pxl.core.Random(); print(r.rand())")
ADD_TEST(PyPXLModuleLoading_XML ${PYTHON_EXECUTABLE} "-c" "import sys; sys.path.insert(0,'${OUTPUT_PATH}'); sys.path.insert(0,'${LIBRARY_OUTPUT_PATH}');import pxl.xml; r = pxl.xml.AnalysisXmlExport()")

if (DOXYGEN_FOUND AND USE_SWIG)
	CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
	add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/xml/index.xml  COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile ${CMAKE_SOURCE_DIR}/xml/include/pxl/xml.hh)
	SET_SOURCE_FILES_PROPERTIES( ${CMAKE_CURRENT_BINARY_DIR}/xml/index.xml PROPERTIES GENERATED 1)

	add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/pxl-doxy.i COMMAND ${PYTHON_EXECUTABLE}  ${CMAKE_CURRENT_SOURCE_DIR}/doxy2swig.py -q  ${CMAKE_CURRENT_BINARY_DIR}/xml/index.xml ${CMAKE_CURRENT_BINARY_DIR}/pxl-doxy.i DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/xml/index.xml)
	add_custom_target(pypxl-doxy DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/pxl-doxy.i)
else()
	MESSAGE(STATUS "Use empty python documentation")
	file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/pxl-doxy.i "")
endif()

INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/pxl-doxy.i DESTINATION ${datadir}/swig)
