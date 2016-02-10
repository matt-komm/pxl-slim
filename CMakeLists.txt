cmake_minimum_required (VERSION 2.6)
project (PXL)

#
# define PXL Version
#
SET(PXLMAJORRELEASEVERSION 3)
SET(PXLMINORRELEASEVERSION 5)
SET(BUGFIXRELEASE 1)

SET(PXLVERSION ${PXLMAJORRELEASEVERSION}.${PXLMINORRELEASEVERSION}.${BUGFIXRELEASE})
SET(PXLMAINVERSION ${PXLMAJORRELEASEVERSION}.${PXLMINORRELEASEVERSION})

ADD_DEFINITIONS(-std=c++0x)

if (NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)
MESSAGE(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")

#
# setup swig
#
OPTION(USE_SWIG "regenerate swig interfaces" ON)
OPTION(ENABLESWIGDOCSTRINGS "Automatically generate docstrings with swig" ON)
OPTION(ENABLETESTING "Build tests and enable test target" ON)

OPTION(ENABLE_COVERAGE_TEST "Build tests and enable test target" OFF)
IF (ENABLE_COVERAGE_TEST)
  MESSAGE(STATUS "Enable coverage test")
  SET(CMAKE_CXX_FLAGS "-fprofile-arcs -ftest-coverage ${CMAKE_CXX_FLAGS}")
  SET(CMAKE_EXE_LINKER_FLAGS "-lgcov ${CMAKE_EXE_LINKER_FLAGS}")
endif()

INCLUDE (Python.cmake)
IF (${PYTHON_DOT_VERSION} VERSION_LESS 2.6)
	MESSAGE(SEND_ERROR " PXL requires Python 2.6 or later. If you have python 2.6 or newer installed but it is not your default python version, you can provide the location of the newer python interpreter e.g. via
	$cmake -DPYTHON_EXECUTABLE=/usr/bin/python2.6")
ENDIF()

if (NOT PYTHON_EXECUTABLE)
	message(FATAL_ERROR "PYTHON EXECUTEABLE NOT FOUND! You can set the location of the python interpreter e.g. via
	$cmake -DPYTHON_EXECUTABLE=/usr/bin/python2.6")
endif()

FIND_PACKAGE(SWIG REQUIRED)
find_package(Doxygen)

ADD_DEFINITIONS(-DPXL_EXPORT)

include(CheckIncludeFile)

MACRO(PXL_CHECK_HEADER filename define)
    check_include_file(${filename} ${define})
    if(${define})
        add_definitions(-D${define})
    endif(${define})
ENDMACRO(PXL_CHECK_HEADER filename define)

PXL_CHECK_HEADER(winsock2.h HAVE_WINSOCK2_H)
PXL_CHECK_HEADER(sys/socket.h HAVE_SYS_SOCKET_H)
PXL_CHECK_HEADER(netinet/in.h HAVE_NETINET_IN_H)
PXL_CHECK_HEADER(unistd.h HAVE_UNISTD_H)
PXL_CHECK_HEADER(arpa/inet.h HAVE_ARPA_INET_H)
PXL_CHECK_HEADER(sys/time.h HAVE_SYS_TIME_H)
PXL_CHECK_HEADER(netdb.h HAVE_NETDB_H)
PXL_CHECK_HEADER(Ws2tcpip.h HAVE_WS2TCPIP_H)

#
# define include directory
#

SET(PXLINCLUDEDIR ${CMAKE_INSTALL_PREFIX}/include/pxl-${PXLMAINVERSION})

SET(PXLLIBSDIR ${CMAKE_INSTALL_PREFIX}/lib)
#
# define the plugindirectory -
#
SET(PXLPLUGINDIR ${CMAKE_INSTALL_PREFIX}/lib/pxl-${PXLMAINVERSION})

#
# define use healpix option
#
OPTION(USE_HEALPIX "Build the healpix library, which is under GPL" TRUE)

#
# the install directories
#
#SET(BIN_INSTALLDIR ${CMAKE_INSTALL_PREFIX}/bin)
#SET(LIB_INSTALLDIR ${CMAKE_INSTALL_PREFIX}/lib)
#SET(HEADER_INSTALLDIR ${CMAKE_INSTALL_PREFIX}/include)

FIND_PACKAGE(Threads)

if (MSVC)
	add_definitions( "/wd4290 /wd4251 /nologo" )
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE -D_SCL_SECURE_NO_WARNINGS)
endif(MSVC)

#
# DUPLICATIONS TO WORK WITH autotools *.in files
#
SET(PXL_VERSION ${PXLVERSION}) 
SET(PXL_LIB_VERSION ${PXLMAINVERSION}) 
SET(includedir ${PXLINCLUDEDIR}) 
SET(libdir ${PXLLIBSDIR}) 
SET(datadir ${CMAKE_INSTALL_PREFIX}/share/pxl) 
SET(exec_prefix ${CMAKE_INSTALL_PREFIX}) 
SET(prefix ${CMAKE_INSTALL_PREFIX}) 

# setup stdint.h
if (MSVC)
	FIND_PATH(STDINT_H_PATH stdint.h lib/stdint)
	MESSAGE(STATUS "Use stdint.h from " ${STDINT_H_PATH})
	include_directories(${STDINT_H_PATH})
endif (MSVC)


#
# check for debug flag
#
OPTION(DEFINE_DEBUG "Build the project using debugging code" OFF)
IF(DEFINE_DEBUG)
    MESSAGE(STATUS "Adding Debug flag...")
    ADD_DEFINITIONS(-DDEBUG)
    SET(CMAKE_BUILD_TYPE Debug)
    MESSAGE(STATUS "Build type is " ${CMAKE_BUILD_TYPE})
ENDIF(DEFINE_DEBUG)

SET(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
SET(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")

if(MSVC)
	SET(OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin/Release")
else(MSVC)
	SET(OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
ENDIF(MSVC)
MESSAGE(STATUS "Output Path: ${OUTPUT_PATH}")

#
# define common macro for installing libraries
#
MACRO(PXL_INSTALL_LIBRARY name)
    SET_TARGET_PROPERTIES(${name} PROPERTIES VERSION ${PXLMAINVERSION})
	if (WIN32)
        INSTALL(TARGETS ${name} ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
        INSTALL(TARGETS ${name} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})
  else (WIN32)
        INSTALL(TARGETS ${name} LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
				INSTALL(DIRECTORY include/pxl DESTINATION ${PXLINCLUDEDIR} FILES_MATCHING PATTERN "*.hh" PATTERN ".*" EXCLUDE)
				
		#
		# automatically generate pkgconfig file and install it
		#
		IF (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${name}.pc.in)
		    CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/${name}.pc.in ${CMAKE_CURRENT_BINARY_DIR}/${name}.pc)
		    INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/${name}.pc DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig)
		ENDIF()
	endif(WIN32)
ENDMACRO(PXL_INSTALL_LIBRARY name)

#
# define common macro for installing executables
#
MACRO(PXL_INSTALL_EXECUTABLE name)
    if (WIN32)
        INSTALL(TARGETS ${name} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})
    else (WIN32)
        INSTALL(TARGETS ${name} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
    endif(WIN32)
ENDMACRO(PXL_INSTALL_EXECUTABLE name)

INCLUDE(Xcode.cmake)

include(FindLibSSH.cmake)
if (LIB_SSH_FOUND)
    add_definitions(-DPXL_ENABLE_SFTP)
    set(PXL_ENABLE_SFTP TRUE)
endif()

include(FindDCAP.cmake)
if (LIB_DCAP_FOUND)
		MESSAGE(STATUS "Enable DCAP")
    add_definitions(-DPXL_ENABLE_DCAP)
    set(PXL_ENABLE_DCAP TRUE)
endif()


#
# add externals to include directory 
#
include_directories (${PXL_SOURCE_DIR}/lib)

#
# add sub projects
#
if (ENABLETESTING)
	enable_testing()
	add_subdirectory (test)
	add_subdirectory (lib/gtest)
endif(ENABLETESTING)

#
# find zlib
#
SET(ZLIB_INCLUDE_DIR)
FIND_PACKAGE(ZLIB)
IF(ZLIB_FOUND)
	MESSAGE(STATUS "Use system zlib")
ELSE(ZLIB_FOUND)
	add_subdirectory (lib/zlib)
	MESSAGE(STATUS "Use internal zlib")
	SET(ZLIB_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/lib/zlib" "${CMAKE_BINARY_DIR}/lib/zlib")
	SET(ZLIB_LIBRARIES zlib)
ENDIF(ZLIB_FOUND)

if (USE_SWIG)
    add_subdirectory (pypxl)
ENDIF()


add_subdirectory (core)
add_subdirectory (doc)
add_subdirectory (hep)
add_subdirectory (xml)
add_subdirectory (astro)
add_subdirectory (scripting)
add_subdirectory (algorithms)
add_subdirectory (modules)
add_subdirectory (pxlrun)
add_subdirectory (examples)
add_subdirectory (skeletons)

INCLUDE(WindowsInstaller.cmake)

IF (UNIX)
	# setup make dist
	add_custom_target(dist
	    cd ${CMAKE_SOURCE_DIR} &&
	    hg archive -X .hgtags -X .hg_archival.txt -X .hgignore -t tar -p pxl-${PXLVERSION} - |
	    gzip > ${CMAKE_BINARY_DIR}/pxl-${PXLVERSION}.tar.gz)

	# setup make distcheck
	add_custom_target(distcheck
	    cd ${CMAKE_BINARY_DIR} &&
	    rm -rf pxl-${PXLVERSION} &&
	    gzip -df pxl-${PXLVERSION}.tar.gz &&
	    tar -xf pxl-${PXLVERSION}.tar &&
	    cd pxl-${PXLVERSION}/ &&
	    cmake . && make && make test &&
	    cd ${CMAKE_BINARY_DIR} &&
	    tar -rf pxl-${PXLVERSION}.tar pxl-${PXLVERSION}/doc/ &&
	    gzip ${CMAKE_BINARY_DIR}/pxl-${PXLVERSION}.tar)
	add_dependencies(distcheck dist)
ENDIF (UNIX)
        
