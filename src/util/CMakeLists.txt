#
# Build the util submodule
#

include_directories(
    ${GLES2_INCLUDE_DIRS}
    ${WAYLAND_CURSOR_SERVER_INCLUDE_DIRS}
    ${WAYLAND_SERVER_SERVER_INCLUDE_DIRS}
    ${UUID_INCLUDE_DIRS}
)

add_definitions(
    ${GLES2_DEFINITIONS}
    ${WAYLAND_CURSOR_SERVER_DEFINITIONS}
    ${WAYLAND_SERVER_SERVER_DEFINITIONS}
    ${UUID_DEFINITIONS}
)

set(SOURCE_FILES
    cleaner.c
    socket.c
    wayland.c
    exec.c
    error.c
)

add_library(util STATIC
    ${SOURCE_FILES}
)

target_link_libraries(util
    ${GLES2_LIBRARIES}
    ${WAYLAND_CURSOR_LIBRARIES}
    ${WAYLAND_SERVER_LIBRARIES}
    ${UUID_LIBRARIES}
)

