# - Try to find gbm
# Once done this will define
#  GBM_FOUND - System has gbm
#  GBM_INCLUDE_DIRS - The gbm include directories
#  GBM_LIBRARIES - The libraries needed for gbm
#  GBM_DEFINITIONS - Compiler switches required for using gbm

find_package(PkgConfig)
pkg_check_modules(PC_GBM QUIET gbm)
set(GBM_DEFINITIONS ${PC_GBM_CFLAGS_OTHER})

find_path(GBM_INCLUDE_DIR gbm.h
    HINTS ${PC_GBM_INCLUDEDIR} ${PC_GBM_INCLUDE_DIRS})

find_library(GBM_LIBRARY gbm
        HINTS ${PC_GBM_LIBDIR} ${PC_GBM_LIBRARY_DIRS})

set(GBM_INCLUDE_DIRS ${GBM_INCLUDE_DIR})
set(GBM_LIBRARIES ${GBM_LIBRARY})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set GBM_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(GBM DEFAULT_MSG
    GBM_INCLUDE_DIR GBM_LIBRARY)

mark_as_advanced(GBM_INCLUDE_DIR GBM_LIBRARY)

