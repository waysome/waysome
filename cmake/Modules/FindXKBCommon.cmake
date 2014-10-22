# - Try to find xkb-common
# Once done this will define
#  XKB_COMMON_FOUND - System has xkb-common
#  XKB_COMMON_INCLUDE_DIRS - The xkb-common include directories
#  XKB_COMMON_LIBRARIES - The libraries needed for xkb-common
#  XKB_COMMON_DEFINITIONS - Compiler switches required for using xkb-common

find_package(PkgConfig)
pkg_check_modules(PC_XKB_COMMON QUIET xkbcommon)
set(XKB_COMMON_DEFINITIONS ${PC_XKB_COMMON_CFLAGS_OTHER})

find_path(XKB_COMMON_INCLUDE_DIR xkbcommon/xkbcommon.h
    HINTS ${PC_XKB_COMMON_INCLUDEDIR} ${PC_XKB_COMMON_INCLUDE_DIRS})

find_library(XKB_COMMON_LIBRARY xkbcommon
        HINTS ${PC_XKB_COMMON_LIBDIR} ${PC_XKB_COMMON_LIBRARY_DIRS})

set(XKB_COMMON_INCLUDE_DIRS ${XKB_COMMON_INCLUDE_DIR})
set(XKB_COMMON_LIBRARIES ${XKB_COMMON_LIBRARY})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set XKB_COMMON_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(XKB_COMMON DEFAULT_MSG
    XKB_COMMON_INCLUDE_DIR XKB_COMMON_LIBRARY)

mark_as_advanced(XKB_COMMON_INCLUDE_DIR XKB_COMMON_LIBRARY)

