#set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
#include(UseDoxygen OPTIONAL)

configure_file(Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
add_custom_target(docPXL COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
add_dependencies(doc docPXL)
