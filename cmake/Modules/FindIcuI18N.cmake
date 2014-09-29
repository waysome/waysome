# - Try to find icu-i18n
# Once done this will define
#  ICU_I18N_FOUND - System has icu-i18n
#  ICU_I18N_INCLUDE_DIRS - The icu-i18n include directories
#  ICU_I18N_LIBRARIES - The libraries needed for icu-i18n
#  ICU_I18N_DEFINITIONS - Compiler switches required for using icu-i18n

find_package(IcuUC)

find_package(PkgConfig)
pkg_check_modules(PC_ICU_I18N QUIET icu-i18n)
set(ICU_I18N_DEFINITIONS ${PC_ICU_I18N_CFLAGS_OTHER})

find_path(ICU_I18N_INCLUDE_DIR unicode/uloc.h
    HINTS ${PC_ICU_I18N_INCLUDEDIR} ${PC_ICU_I18N_INCLUDE_DIRS})

find_library(ICU_I18N_LIBRARY icui18n
        HINTS ${PC_ICU_I18N_LIBDIR} ${PC_ICU_I18N_LIBRARY_DIRS})

set(ICU_I18N_INCLUDE_DIRS ${ICU_I18N_INCLUDE_DIR} ${ICU_UC_INCLUDE_DIRS})
set(ICU_I18N_LIBRARIES ${ICU_I18N_LIBRARY} ${ICU_UC_LIBRARIES})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set ICU_I18N_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(IcuI18N DEFAULT_MSG
    ICU_I18N_INCLUDE_DIR ICU_I18N_LIBRARY)

mark_as_advanced(ICU_I18N_INCLUDE_DIR ICU_I18N_LIBRARY)

