# - Try to find the "latex2man" utility
# Once done this will define
#  LATEX2MAN_FOUND - System has latex2man
#  LATEX2MAN_EXECUTABLE - The latex2man program

find_program(LATEX2MAN_EXECUTABLE latex2man)

include(FindPackageHandleStandardArgs)

# handle the QUIETLY and REQUIRED arguments and set LATEX2MAN_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(Latex2man DEFAULT_MSG LATEX2MAN_EXECUTABLE)

mark_as_advanced(LATEX2MAN_EXECUTABLE)



