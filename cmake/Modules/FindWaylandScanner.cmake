# - Try to find wayland-scanner
# Once done this will define
#  WAYLAND_SCANNER_FOUND - System has wayland-scanner
#  WAYLAND_SCANNER_EXE - The wayland-scanner executable

find_program(WAYLAND_SCANNER_EXE wayland-scanner)

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set WAYLAND_SCANNER_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(WaylandScanner DEFAULT_MSG WAYLAND_SCANNER_EXE)

mark_as_advanced(WAYLAND_SCANNER_EXE)

