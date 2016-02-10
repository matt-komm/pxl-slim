#
# use local include directory
#
include_directories (
    ${PXL_SOURCE_DIR}/core/include
    ${PXL_SOURCE_DIR}/hep/include
    ${PXL_SOURCE_DIR}/astro/include
    ${PXL_SOURCE_DIR}/modules/include
    ${PXL_SOURCE_DIR}/algorithms/include
    ${PXL_SOURCE_DIR}/scripting/include
    ${PXL_SOURCE_DIR}/xml/include
    ${CMAKE_BINARY_DIR}/include
    ${PXL_SOURCE_DIR}/lib/gtest/include
    ${PXL_SOURCE_DIR}/test
)

SET(THREAD_LIB "pthread")
if (MSVC)
	SET(THREAD_LIB "")
endif(MSVC)

#link_directories(${CMAKE_BINARY_DIR}/bin)

add_definitions(-DTESTSOURCEDIR="${CMAKE_CURRENT_SOURCE_DIR}"
	-DLIBRARY_OUTPUT_PATH="${LIBRARY_OUTPUT_PATH}"
	)


ENABLE_TESTING()
INCLUDE(CTest)

add_executable(test_core test_core.cc)
add_dependencies(test_core
    pxl-core
) 

target_link_libraries (test_core
    gtest
	gtest_main
    pxl-core
    ${THREAD_LIB}
)



add_executable(test_scripting test_scripting.cc)
add_dependencies(test_scripting
    pxl-core
    pxl-scripting
) 

target_link_libraries (test_scripting
    pxl-core
    pxl-scripting
		pxl-xml
    gtest
		gtest_main
    ${THREAD_LIB}
)



add_executable(test_modules test_modules.cc)
add_dependencies(test_modules
    pxl-core
    pxl-modules
) 
target_link_libraries (test_modules
    pxl-core
    pxl-modules
    gtest
		gtest_main
    ${THREAD_LIB}
)

add_executable(test_xml test_xml.cc)
add_dependencies(test_xml
    pxl-core
    pxl-xml
    pxl-modules
) 
target_link_libraries (test_xml
    pxl-core
    pxl-xml
    pxl-modules
    gtest
		gtest_main
    ${THREAD_LIB}
)


add_executable(test_astro test_astro.cc)
add_dependencies(test_astro
    pxl-astro
    pxl-core
) 
target_link_libraries (test_astro
    pxl-astro
    pxl-core
    gtest
		gtest_main
     ${THREAD_LIB}
)

add_executable(test_hep test_hep.cc)
add_dependencies(test_hep
    pxl-core
    pxl-hep
) 
target_link_libraries (test_hep
    pxl-core
    pxl-hep
    gtest
		gtest_main
     ${THREAD_LIB}
)

#
# test_healpix
#
if (USE_HEALPIX)
	add_executable(test_healpix test_healpix.cc)
	add_dependencies(test_healpix
			pxl-core
		  pxl-astro
			pxl-healpix
	) 
	target_link_libraries (test_healpix
			pxl-core
		  pxl-astro
			pxl-healpix
			gtest
		 gtest_main
			${THREAD_LIB}
	)

	#Test healpix plugin 
	ADD_TEST(test_healpix ${EXECUTABLE_OUTPUT_PATH}/test_healpix)

	CONFIGURE_FILE(test_healpix_pythonInterface.py.in ${CMAKE_CURRENT_BINARY_DIR}/test_healpix_pythonInterface.py) 
	#Test python wrapper
	ADD_TEST(test_healpix_pythonInterface ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/test_healpix_pythonInterface.py)
endif (USE_HEALPIX)

#
# add executable using the sources
#

add_executable(heptest HepTest.cc)
add_library(moduletest SHARED ModuleTest.cc)

add_dependencies(heptest
    pxl-core
    pxl-hep
	pxl-astro
) 
add_dependencies(moduletest 
    pxl-core
    pxl-modules
)


target_link_libraries (heptest
    pxl-core
    pxl-xml
    pxl-modules
)

target_link_libraries (moduletest
    pxl-core
    pxl-xml
    pxl-modules
)


#
# link to pxl, hep
#
target_link_libraries (heptest
    pxl-core
    pxl-hep
	pxl-astro
)


#Test core 
ADD_TEST(test_core ${EXECUTABLE_OUTPUT_PATH}/test_core)

#Test core 
ADD_TEST(test_scripting ${EXECUTABLE_OUTPUT_PATH}/test_scripting)

#Test astro 
ADD_TEST(test_astro ${EXECUTABLE_OUTPUT_PATH}/test_astro)

#Test hep 
ADD_TEST(test_hep ${EXECUTABLE_OUTPUT_PATH}/test_hep)

#Test modules 
ADD_TEST(test_modules ${EXECUTABLE_OUTPUT_PATH}/test_modules)

#Test xml 
ADD_TEST(test_xml ${EXECUTABLE_OUTPUT_PATH}/test_xml)

#Tests HEP (and many core functionalities)
ADD_TEST(Hep ${EXECUTABLE_OUTPUT_PATH}/heptest)

#test loading and executing of a module, defined in ModuleTest
ADD_TEST(LoadTestModule ${PYTHON_EXECUTABLE} "-c" "import sys; sys.path.insert(0,'${OUTPUT_PATH}'); from pxl import core, modules; core.LogDispatcher.instance().enableConsoleLogHandler(core.LOG_LEVEL_DEBUG); core.PluginManager.instance().loadPluginsFromDirectory('${OUTPUT_PATH}'); analysis = modules.Analysis(); analysis.addModule('Example Module','TM', '0'); analysis.run()")

#Test analysis execution with pxlrun
ADD_TEST(pxlrun ${EXECUTABLE_OUTPUT_PATH}/pxlrun -p ${CMAKE_CURRENT_SOURCE_DIR}/count.xml -pp=${OUTPUT_PATH})

#Test python wrapper
MACRO(ADDPYTHONTEST name)
  IF(${PYTHON_MAJOR_VERSION} STREQUAL "3")
      CONFIGURE_FILE(${name}.py3.in ${CMAKE_CURRENT_BINARY_DIR}/${name}.py)
  ELSE()
      CONFIGURE_FILE(${name}.py.in ${CMAKE_CURRENT_BINARY_DIR}/${name}.py)
  ENDIF()
  ADD_TEST(${name} ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/${name}.py)
ENDMACRO(ADDPYTHONTEST name)

ADDPYTHONTEST(pypxl_testModules)
ADDPYTHONTEST(pypxl_testXml)
ADDPYTHONTEST(pypxl_testCore)
ADDPYTHONTEST(pypxl_testAstro)
ADDPYTHONTEST(pypxl_testHep)
if (PXL_ENABLE_ROOT)
    ADDPYTHONTEST(pypxl_testRoot)
endif()

