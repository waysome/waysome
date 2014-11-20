#
# Build the util submodule
#

include_directories(
    ${WAYLAND_CURSOR_SERVER_LIBRARIES}
    ${WAYLAND_SERVER_SERVER_LIBRARIES}
    ${UUID_INCLUDE_DIRS}
)

add_definitions(
    ${WAYLAND_CURSOR_SERVER_DEFINITIONS}
    ${WAYLAND_SERVER_SERVER_DEFINITIONS}
    ${UUID_DEFINITIONS}
)

set(SOURCE_FILES
    cleaner.c
    socket.c
    wayland.c
    exec.c
)

add_library(util STATIC
    ${SOURCE_FILES}
)

target_link_libraries(util
    ${WAYLAND_CURSOR_LIBRARIES}
    ${WAYLAND_SERVER_LIBRARIES}
    ${UUID_LIBRARIES}
)

