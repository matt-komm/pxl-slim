IF (MSVC)
	GET_FILENAME_COMPONENT(WIX_INSTALL_ROOT "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows Installer XML\\3.0;InstallRoot]" ABSOLUTE CACHE)
	MESSAGE(STATUS "Windows Installer XML: ${WIX_INSTALL_ROOT}")
	
	FIND_PROGRAM( CANDLE_EXECUTABLE candle
		PATHS ${WIX_INSTALL_ROOT}
		REQUIRED
	) 

	FIND_PROGRAM( LIGHT_EXECUTABLE light
		PATHS  ${WIX_INSTALL_ROOT}
	) 

	CONFIGURE_FILE(pypxl.wxs.in ${CMAKE_BINARY_DIR}/pypxl.wxs)

	add_custom_target( windows_installer_candle
	    ${CANDLE_EXECUTABLE} ${CMAKE_BINARY_DIR}/pypxl.wxs
		DEPENDS ${CMAKE_BINARY_DIR}/pypxl.wxs
	)
	add_dependencies(windows_installer_candle pxlrun pxl-modules)

	add_custom_target( windows_installer
		${LIGHT_EXECUTABLE} -ext WixUIExtension ${CMAKE_BINARY_DIR}/pypxl.wixobj -o ${CMAKE_BINARY_DIR}/PyPXL-Py${PYTHON_DOT_VERSION}-${PXLVERSION}.msi
	)
	add_dependencies(windows_installer windows_installer_candle)
ENDIF(MSVC)