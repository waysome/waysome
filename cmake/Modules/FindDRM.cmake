# - Try to find libdrm
# Once done this will define
#  DRM_FOUND - System has libdrm
#  DRM_INCLUDE_DIRS - The libdrm include directories
#  DRM_LIBRARIES - The libraries needed for libdrm
#  DRM_DEFINITIONS - Compiler switches required for using libdrm

find_package(PkgConfig)
pkg_check_modules(PC_DRM QUIET libdrm)
set(DRM_DEFINITIONS ${PC_DRM_CFLAGS_OTHER})

find_path(DRM_INCLUDE_DIR drm.h
    HINTS ${PC_DRM_INCLUDEDIR} ${PC_DRM_INCLUDE_DIRS})

find_library(DRM_LIBRARY drm
        HINTS ${PC_DRM_LIBDIR} ${PC_DRM_LIBRARY_DIRS})

set(DRM_INCLUDE_DIRS ${DRM_INCLUDE_DIR})
set(DRM_LIBRARIES ${DRM_LIBRARY})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set DRM_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(DRM DEFAULT_MSG
    DRM_INCLUDE_DIR DRM_LIBRARY)

mark_as_advanced(DRM_INCLUDE_DIR DRM_LIBRARY)

