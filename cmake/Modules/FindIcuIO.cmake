# - Try to find icu-io
# Once done this will define
#  ICU_IO_FOUND - System has icu-io
#  ICU_IO_INCLUDE_DIRS - The icu-io include directories
#  ICU_IO_LIBRARIES - The libraries needed for icu-io
#  ICU_IO_DEFINITIONS - Compiler switches required for using icu-io

find_package(IcuI18N)

find_package(PkgConfig)
pkg_check_modules(PC_ICU_IO QUIET icu-io)
set(ICU_IO_DEFINITIONS ${PC_ICU_IO_CFLAGS_OTHER})

find_path(ICU_IO_INCLUDE_DIR unicode/ustdio.h
    HINTS ${PC_ICU_IO_INCLUDEDIR} ${PC_ICU_IO_INCLUDE_DIRS})

find_library(ICU_IO_LIBRARY icuio
    HINTS ${PC_ICU_IO_LIBDIR} ${PC_ICU_IO_LIBRARY_DIRS})

set(ICU_IO_INCLUDE_DIRS ${ICU_IO_INCLUDE_DIR} ${ICU_I18N_INCLUDE_DIRS})
set(ICU_IO_LIBRARIES ${ICU_IO_LIBRARY} ${ICU_I18N_LIBRARIES})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set ICU_IO_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(IcuIO DEFAULT_MSG
    ICU_IO_INCLUDE_DIR ICU_IO_LIBRARY)

mark_as_advanced(ICU_IO_INCLUDE_DIR ICU_IO_LIBRARY)

