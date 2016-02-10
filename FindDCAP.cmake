# - Try to find dcap 
# Once done, this will define
#
#  LIB_DCAP_FOUND - system has PXL 
#  LIB_DCAPH_INCLUDE_DIRS - the PXL include directories
#  LIB_DCAP_LIBRARIES - link these to use PXL 

# library 
find_library(LIB_DCAP_LIBRARIES NAMES dcap HINTS "/opt/d-cache/dcap/lib" ENV LD_LIBRARY_PATH)
get_filename_component(LIB_DCAP_LIBDIR ${LIB_DCAP_LIBRARIES} PATH)   # find directory name of library file as hint for include dir

# Include dir
find_path(LIB_DCAP_INCLUDE_DIR NAMES dcap.h HINTS ${LIB_DCAP_LIBDIR}/../include)


if (LIB_DCAP_INCLUDE_DIR AND LIB_DCAP_LIBRARIES)
    set(LIB_DCAP_FOUND TRUE)
    message(STATUS "Found dcap: ${LIB_DCAP_INCLUDE_DIR}") 
else()
    set(LIB_DCAP_FOUND FALSE)
    message(STATUS "NOT Found: dcap") 
endif()
