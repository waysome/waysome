add_custom_command(OUTPUT xdg-shell-source.c xdg-shell-header.h
    COMMAND ${WAYLAND_SCANNER_EXE} code < xdg-shell-protocol.xml > xdg-shell-source.c
    COMMAND ${WAYLAND_SCANNER_EXE} server-header < xdg-shell-protocol.xml > xdg-shell-header.h
    DEPENDS xdg-shell-protocol.xml VERBATIM)

set(SOURCE_FILES
    xdg-shell-source.c
)

add_library(protocol OBJECT
    ${SOURCE_FILES}
)
