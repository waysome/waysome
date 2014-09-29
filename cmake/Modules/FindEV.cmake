# - Try to find ev
# Once done this will define
#  EV_FOUND - System has libev
#  EV_INCLUDE_DIRS - The libev include directories
#  EV_LIBRARIES - The libraries needed for libev
#  EV_DEFINITIONS - Compiler switches required for using libev

find_package(PkgConfig)
pkg_check_modules(PC_EV QUIET libev)
set(EV_DEFINITIONS ${PC_EV_CFLAGS_OTHER})

find_path(EV_INCLUDE_DIR ev.h
    HINTS ${PC_EV_INCLUDEDIR} ${PC_EV_INCLUDE_DIRS})

find_library(EV_LIBRARY ev
        HINTS ${PC_EV_LIBDIR} ${PC_EV_LIBRARY_DIRS})

set(EV_INCLUDE_DIRS ${EV_INCLUDE_DIR})
set(EV_LIBRARIES ${EV_LIBRARY})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set EV_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(EV DEFAULT_MSG
    EV_INCLUDE_DIR EV_LIBRARY)

mark_as_advanced(EV_INCLUDE_DIR EV_LIBRARY)

