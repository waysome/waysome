# - Try to find libevdev
# Once done this will define
#  EVDEV_FOUND - System has libevdev
#  EVDEV_INCLUDE_DIRS - The libevdev include directories
#  EVDEV_LIBRARIES - The libraries needed for libevdev
#  EVDEV_DEFINITIONS - Compiler switches required for using libevdev

find_package(PkgConfig)
pkg_check_modules(PC_EVDEV QUIET libevdev)
set(EVDEV_DEFINITIONS ${PC_EVDEV_CFLAGS_OTHER})

find_path(EVDEV_INCLUDE_DIR libevdev/libevdev.h
    HINTS ${PC_EVDEV_INCLUDEDIR} ${PC_EVDEV_INCLUDE_DIRS})

find_library(EVDEV_LIBRARY libevdev.so
        HINTS ${PC_EVDEV_LIBDIR} ${PC_EVDEV_LIBRARY_DIRS})

set(EVDEV_INCLUDE_DIRS ${EVDEV_INCLUDE_DIR})
set(EVDEV_LIBRARIES ${EVDEV_LIBRARY})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set EVDEV_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(EVDEV DEFAULT_MSG
    EVDEV_INCLUDE_DIR EVDEV_LIBRARY)

mark_as_advanced(EVDEV_INCLUDE_DIR EVDEV_LIBRARY)

