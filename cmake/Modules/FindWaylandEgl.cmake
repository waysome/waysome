# - Try to find wayland-egl
# Once done this will define
#  WAYLAND_EGL_FOUND - System has wayland-egl
#  WAYLAND_EGL_INCLUDE_DIRS - The wayland-egl include directories
#  WAYLAND_EGL_DEFINITIONS - Compiler switches required for using wayland-egl

find_package(PkgConfig)
pkg_check_modules(PC_WAYLAND_EGL QUIET wayland-egl)
set(WAYLAND_EGL_DEFINITIONS ${PC_WAYLAND_EGL_CFLAGS_OTHER})

find_path(WAYLAND_EGL_INCLUDE_DIR wayland-egl.h
    HINTS ${PC_WAYLAND_EGL_INCLUDEDIR} ${PC_WAYLAND_EGL_INCLUDE_DIRS})

set(WAYLAND_EGL_INCLUDE_DIRS ${WAYLAND_EGL_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set WAYLAND_EGL_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(WaylandEgl DEFAULT_MSG WAYLAND_EGL_INCLUDE_DIR)

mark_as_advanced(WAYLAND_EGL_INCLUDE_DIR)

