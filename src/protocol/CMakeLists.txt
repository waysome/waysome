add_custom_command(OUTPUT xdg-shell-source.c xdg-shell-header.h
    COMMAND ${WAYLAND_SCANNER_EXE} code < xdg-shell-protocol.xml > xdg-shell-source.c
    COMMAND ${WAYLAND_SCANNER_EXE} server-header < xdg-shell-protocol.xml > xdg-shell-header.h
    DEPENDS xdg-shell-protocol.xml VERBATIM)

include_directories(
    ${WAYLAND_CURSOR_INCLUDE_DIRS}
    ${WAYLAND_SERVER_INCLUDE_DIRS}
)

add_definitions(
    ${WAYLAND_CURSOR_DEFINITIONS}
    ${WAYLAND_SERVER_DEFINITIONS}
)

set(SOURCE_FILES
    xdg-shell-source.c
)

add_library(protocol STATIC
    ${SOURCE_FILES}
)

target_link_libraries(protocol
    ${WAYLAND_CURSOR_LIBRARIES}
    ${WAYLAND_SERVER_LIBRARIES}
)

