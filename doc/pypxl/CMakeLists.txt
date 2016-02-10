configure_file(Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
add_custom_target(docPyPXL  COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
add_dependencies(doc docPyPXL)

#if (USE_SWIG)
#    add_custom_target(docStringsForPyPXL  ${CMAKE_CURRENT_SOURCE_DIR}/doxy2swig.py  ${CMAKE_BINARY_DIR}/doc/pxl/xml/index.xml  ${CMAKE_SOURCE_DIR}/pypxl/src/pxl-doxy.i)
#    add_dependencies(docStringsForPyPXL docPXL)
#    add_dependencies(doc docStringsForPyPXL)
#endif()
