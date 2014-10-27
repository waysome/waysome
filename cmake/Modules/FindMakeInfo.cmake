# - Try to find the "makeinfo" utility
# Once done this will define
#  MAKEINFO_FOUND - System has makeinfo
#  MAKEINFO_EXECUTABLE - The makeinfo program

find_program(MAKEINFO_EXECUTABLE makeinfo)

include(FindPackageHandleStandardArgs)

# handle the QUIETLY and REQUIRED arguments and set MAKEINFO_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(MakeInfo DEFAULT_MSG MAKEINFO_EXECUTABLE)

mark_as_advanced(MAKEINFO_EXECUTABLE)



