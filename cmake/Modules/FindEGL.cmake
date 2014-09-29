# - Try to find egl
# Once done this will define
#  EGL_FOUND - System has egl
#  EGL_INCLUDE_DIRS - The egl include directories
#  EGL_LIBRARIES - The libraries needed for egl
#  EGL_DEFINITIONS - Compiler switches required for using egl

find_package(PkgConfig)
pkg_check_modules(PC_EGL QUIET egl)
set(EGL_DEFINITIONS ${PC_EGL_CFLAGS_OTHER})

find_path(EGL_INCLUDE_DIR EGL/egl.h
    HINTS ${PC_EGL_INCLUDEDIR} ${PC_EGL_INCLUDE_DIRS})

find_library(EGL_LIBRARY EGL
        HINTS ${PC_EGL_LIBDIR} ${PC_EGL_LIBRARY_DIRS})

set(EGL_INCLUDE_DIRS ${EGL_INCLUDE_DIR})
set(EGL_LIBRARIES ${EGL_LIBRARY})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set EGL_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(EGL DEFAULT_MSG
    EGL_INCLUDE_DIR EGL_LIBRARY)

mark_as_advanced(EGL_INCLUDE_DIR EGL_LIBRARY)

