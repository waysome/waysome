# - Try to find wayland-cursor
# Once done this will define
#  WAYLAND_CURSOR_FOUND - System has wayland-cursor
#  WAYLAND_CURSOR_INCLUDE_DIRS - The wayland-cursor include directories
#  WAYLAND_CURSOR_LIBRARIES - The libraries needed for wayland-cursor
#  WAYLAND_CURSOR_DEFINITIONS - Compiler switches required for using
#                               wayland-cursor

find_package(PkgConfig)
pkg_check_modules(PC_WAYLAND_CURSOR QUIET wayland-cursor)
set(WAYLAND_CURSOR_DEFINITIONS ${PC_WAYLAND_CURSOR_CFLAGS_OTHER})

find_path(WAYLAND_CURSOR_INCLUDE_DIR wayland-cursor.h
    HINTS ${PC_WAYLAND_CURSOR_INCLUDEDIR} ${PC_WAYLAND_CURSOR_INCLUDE_DIRS})

find_library(WAYLAND_CURSOR_LIBRARY wayland-cursor
        HINTS ${PC_WAYLAND_CURSOR_LIBDIR} ${PC_WAYLAND_CURSOR_LIBRARY_DIRS})

set(WAYLAND_CURSOR_INCLUDE_DIRS ${WAYLAND_CURSOR_INCLUDE_DIR})
set(WAYLAND_CURSOR_LIBRARIES ${WAYLAND_CURSOR_LIBRARY})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set WAYLAND_CURSOR_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(WaylandCursor DEFAULT_MSG
    WAYLAND_CURSOR_INCLUDE_DIR WAYLAND_CURSOR_LIBRARY)

mark_as_advanced(WAYLAND_CURSOR_INCLUDE_DIR WAYLAND_CURSOR_LIBRARY)

