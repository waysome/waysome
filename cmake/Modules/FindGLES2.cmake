# - Try to find gles2
# Once done this will define
#  GLES2_FOUND - System has gles2
#  GLES2_INCLUDE_DIRS - The gles2 include directories
#  GLES2_LIBRARIES - The libraries needed for gles2
#  GLES2_DEFINITIONS - Compiler switches required for using gles2

find_package(PkgConfig)
pkg_check_modules(PC_GLES2 QUIET glesv2)
set(GLES2_DEFINITIONS ${PC_GLES2_CFLAGS_OTHER})

find_path(GLES2_INCLUDE_DIR GLES2/gl2.h
    HINTS ${PC_GLES2_INCLUDEDIR} ${PC_GLES2_INCLUDE_DIRS})

find_library(GLES2_LIBRARY GLESv2
        HINTS ${PC_GLES2_LIBDIR} ${PC_GLES2_LIBRARY_DIRS})

set(GLES2_INCLUDE_DIRS ${GLES2_INCLUDE_DIR})
set(GLES2_LIBRARIES ${GLES2_LIBRARY})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set GLES2_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(GLES2 DEFAULT_MSG
    GLES2_INCLUDE_DIR GLES2_LIBRARY)

mark_as_advanced(GLES2_INCLUDE_DIR GLES2_LIBRARY)

