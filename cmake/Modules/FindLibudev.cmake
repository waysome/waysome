# - Try to find libudev
# Once done this will define
#  LIBUDEV_FOUND - System has libudev
#  LIBUDEV_INCLUDE_DIRS - The libudev include directories
#  LIBUDEV_LIBRARIES - The libraries needed for libudev
#  LIBUDEV_DEFINITIONS - Compiler switches required for using
#                               libudev

find_package(PkgConfig)
pkg_check_modules(PC_LIBUDEV QUIET libudev)
set(LIBUDEV_DEFINITIONS ${PC_LIBUDEV_CFLAGS_OTHER})

find_path(LIBUDEV_INCLUDE_DIR libudev.h
    HINTS ${PC_LIBUDEV_INCLUDEDIR} ${PC_LIBUDEV_INCLUDE_DIRS})

find_library(LIBUDEV_LIBRARY udev
        HINTS ${PC_LIBUDEV_LIBDIR} ${PC_LIBUDEV_LIBRARY_DIRS})

set(LIBUDEV_INCLUDE_DIRS ${LIBUDEV_INCLUDE_DIR})
set(LIBUDEV_LIBRARIES ${LIBUDEV_LIBRARY})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBUDEV_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(Libudev DEFAULT_MSG
    LIBUDEV_INCLUDE_DIR LIBUDEV_LIBRARY)

mark_as_advanced(LIBUDEV_INCLUDE_DIR LIBUDEV_LIBRARY)

