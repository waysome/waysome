# - Try to find icu-uc
# Once done this will define
#  ICU_UC_FOUND - System has icu-uc
#  ICU_UC_INCLUDE_DIRS - The icu-uc include directories
#  ICU_UC_LIBRARIES - The libraries needed for icu-uc
#  ICU_UC_DEFINITIONS - Compiler switches required for using icu-uc

find_package(PkgConfig)
pkg_check_modules(PC_ICU_UC QUIET icu-uc)
set(ICU_UC_DEFINITIONS ${PC_ICU_UC_CFLAGS_OTHER})

find_path(ICU_UC_INCLUDE_DIR unicode/ustring.h
    HINTS ${PC_ICU_UC_INCLUDEDIR} ${PC_ICU_UC_INCLUDE_DIRS})

find_library(ICU_UC_LIBRARY icuuc
        HINTS ${PC_ICU_UC_LIBDIR} ${PC_ICU_UC_LIBRARY_DIRS})

find_library(ICU_DATA_LIBRARY icudata
        HINTS ${PC_ICU_UC_LIBDIR} ${PC_ICU_UC_LIBRARY_DIRS})

set(ICU_UC_INCLUDE_DIRS ${ICU_UC_INCLUDE_DIR})
set(ICU_UC_LIBRARIES ${ICU_UC_LIBRARY} ${ICU_DATA_LIBRARY})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set ICU_UC_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(IcuUC DEFAULT_MSG
    ICU_UC_INCLUDE_DIR ICU_UC_LIBRARY)

mark_as_advanced(ICU_UC_INCLUDE_DIR ICU_UC_LIBRARY)

