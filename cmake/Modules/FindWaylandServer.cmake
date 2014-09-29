# - Try to find wayland-server
# Once done this will define
#  WAYLAND_SERVER_FOUND - System has wayland-server
#  WAYLAND_SERVER_INCLUDE_DIRS - The wayland-server include directories
#  WAYLAND_SERVER_LIBRARIES - The libraries needed for wayland-server
#  WAYLAND_SERVER_DEFINITIONS - Compiler switches required for using
#                               wayland-server

find_package(PkgConfig)
pkg_check_modules(PC_WAYLAND_SERVER QUIET wayland-server)
set(WAYLAND_SERVER_DEFINITIONS ${PC_WAYLAND_SERVER_CFLAGS_OTHER})

find_path(WAYLAND_SERVER_INCLUDE_DIR wayland-server.h
    HINTS ${PC_WAYLAND_SERVER_INCLUDEDIR} ${PC_WAYLAND_SERVER_INCLUDE_DIRS})

find_library(WAYLAND_SERVER_LIBRARY wayland-server
        HINTS ${PC_WAYLAND_SERVER_LIBDIR} ${PC_WAYLAND_SERVER_LIBRARY_DIRS})

set(WAYLAND_SERVER_INCLUDE_DIRS ${WAYLAND_SERVER_INCLUDE_DIR})
set(WAYLAND_SERVER_LIBRARIES ${WAYLAND_SERVER_LIBRARY})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set WAYLAND_SERVER_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(WaylandServer DEFAULT_MSG
    WAYLAND_SERVER_INCLUDE_DIR WAYLAND_SERVER_LIBRARY)

mark_as_advanced(WAYLAND_SERVER_INCLUDE_DIR WAYLAND_SERVER_LIBRARY)

