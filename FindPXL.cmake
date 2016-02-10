# - Try to find PXL 
# Once done, this will define
#
#  PXL_FOUND - system has PXL 
#  PXL_INCLUDE_DIRS - the PXL include directories
#  PXL_LIBRARIES_DIR - link these to use PXL 
#  PXL_PLUGIN_INSTALL_PATH - path to the default plugins (e.g.
#														 $HOME/.pxl-3.0/plugins)
# Note that this only configures the pxl-core system to add a pxl
# plugin, use the PXL_ADD_PLUGIN(name), where name is e.g. pxl-astro


# Use pkg-config to get hints about paths

find_program(PXLRUN_PATH pxlrun)

OPTION(INSTALL_PXL_USER "Install PXL modules in user plugin path" ON)

if (PXLRUN_PATH)
	SET(PXL_FOUND TRUE)
  IF (INSTALL_PXL_USER)
    execute_process (COMMAND ${PXLRUN_PATH} --getUserPluginPath
	  	OUTPUT_VARIABLE PXL_PLUGIN_INSTALL_PATH
	  	OUTPUT_STRIP_TRAILING_WHITESPACE)
  ELSE(INSTALL_PXL_USER)
    execute_process (COMMAND ${PXLRUN_PATH} --getDefaultPluginPath
	  	OUTPUT_VARIABLE PXL_PLUGIN_INSTALL_PATH
	  	OUTPUT_STRIP_TRAILING_WHITESPACE)
  ENDIF(INSTALL_PXL_USER)
  MESSAGE(STATUS Install\ PXL-modules\ to \ ${PXL_PLUGIN_INSTALL_PATH}  )

	execute_process (COMMAND ${PXLRUN_PATH} --incdir
		OUTPUT_VARIABLE PXL_INCLUDE_DIRS 
		OUTPUT_STRIP_TRAILING_WHITESPACE)

	execute_process (COMMAND ${PXLRUN_PATH} --libdir
		OUTPUT_VARIABLE PXL_LIBRARIES_DIR
		OUTPUT_STRIP_TRAILING_WHITESPACE)
else(PXLRUN_PATH)
	SET(PXL_FOUND FALSE)
endif(PXLRUN_PATH)

#macro to add a pxl plugin
MACRO(ADD_PXL_PLUGIN name)
	MESSAGE(STATUS Adding\ PXL\ plugin \ ${name})
  find_library(lib_${name}_library
    NAMES ${name}
		HINTS ${PXL_LIBRARIES_DIR}
    )
  LIST(APPEND PXL_LIBRARIES ${lib_${name}_library}) 
ENDMACRO(ADD_PXL_PLUGIN name)

#macro to add a new module to cmake
MACRO(ADD_PXL_MODULE PXLMODULENAME CPPFILE)
	IF(APPLE)
	add_library(${PXLMODULENAME} SHARED MODULE
		${CPPFILE}
		)
	ELSE(APPLE)
		add_library(${PXLMODULENAME} SHARED 
			${CPPFILE}
			)
	ENDIF(APPLE)
	
	target_link_libraries (${PXLMODULENAME}
		${PXL_LIBRARIES}
		)
	
	SET_TARGET_PROPERTIES(${PXLMODULENAME} PROPERTIES COMPILE_FLAGS -fPIC)
	INSTALL(TARGETS ${PXLMODULENAME} LIBRARY DESTINATION ${PXL_PLUGIN_INSTALL_PATH})

ENDMACRO(ADD_PXL_MODULE name)

#Add pxl core plugin by default
ADD_PXL_PLUGIN(pxl-core)


