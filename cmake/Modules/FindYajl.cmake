# - Try to find yajl
# Once done this will define
#  YAJL_FOUND - System has yajl
#  YAJL_INCLUDE_DIRS - The yajl include directories
#  YAJL_LIBRARIES - The libraries needed for yajl
#  YAJL_DEFINITIONS - Compiler switches required for using yajl

find_package(PkgConfig)
pkg_check_modules(PC_YAJL QUIET yajl)
set(YAJL_DEFINITIONS ${PC_YAJL_CFLAGS_OTHER})

find_path(YAJL_INCLUDE_DIR yajl/yajl_parse.h
    HINTS ${PC_YAJL_INCLUDEDIR} ${PC_YAJL_INCLUDE_DIRS})

find_library(YAJL_LIBRARY yajl
        HINTS ${PC_YAJL_LIBDIR} ${PC_YAJL_LIBRARY_DIRS})

set(YAJL_INCLUDE_DIRS ${YAJL_INCLUDE_DIR})
set(YAJL_LIBRARIES ${YAJL_LIBRARY})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set YAJL_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(Yajl DEFAULT_MSG
    YAJL_INCLUDE_DIR YAJL_LIBRARY)

mark_as_advanced(YAJL_INCLUDE_DIR YAJL_LIBRARY)

