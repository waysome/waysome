# - Try to find the "uuid" library
# Once done this will define
#  UUID_FOUND - System has libuuid
#  UUID_INCLUDE_DIRS - The libuuid include directories
#  UUID_LIBRARIES - The libraries needed to use libuuid
#  UUID_DEFINITIONS - Compiler switches required for using libuuid

find_package(PkgConfig)
pkg_check_modules(PC_UUID QUIET uuid)

set(UUID_DEFINITIONS ${PC_UUID_CFLAGS_OTHER})

find_path(UUID_INCLUDE_DIR uuid/uuid.h
    HINTS ${PC_UUID_INCLUDEDIR} ${PC_UUID_INCLUDE_DIRS}
    PATH_SUFFIXES ..)

find_library(UUID_LIBRARY NAMES uuid
    HINTS ${PC_UUID_LIBDIR} ${PC_UUID_LIBRARY_DIRS})

set(UUID_LIBRARIES ${UUID_LIBRARY})
set(UUID_INCLUDE_DIRS ${UUID_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)

# handle the QUIETLY and REQUIRED arguments and set UUID_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(UUID DEFAULT_MSG UUID_LIBRARY UUID_INCLUDE_DIR)

mark_as_advanced(UUID_INCLUDE_DIR UUID_LIBRARY)



