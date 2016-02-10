# - Try to find libssh2 
# Once done, this will define
#
#  LIB_SSH_FOUND - system has PXL 
#  LIB_SSH_INCLUDE_DIRS - the PXL include directories
#  LIB_SSH_LIBRARIES - link these to use PXL 

# Include dir
find_path(LIB_SSH_INCLUDE_DIR NAMES libssh2.h)

# library 
find_library(LIB_SSH_LIBRARIES NAMES ssh2)

if (LIB_SSH_INCLUDE_DIR AND LIB_SSH_LIBRARIES)
    set(LIB_SSH_FOUND TRUE)
    message(STATUS "Found libssh2: ${LIB_SSH_INCLUDE_DIR}") 
else()
    set(LIB_SSH_FOUND FALSE)
    message(STATUS "NOT Found: libssh2") 
endif()
