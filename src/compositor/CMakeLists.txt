#
# Build the compositor submodule
#

include_directories(
    ${DRM_INCLUDE_DIRS}
    ${EGL_INCLUDE_DIRS}
    ${GBM_INCLUDE_DIRS}
    ${PNG_INCLUDE_DIRS}
    ${WAYLAND_CURSOR_INCLUDE_DIRS}
    ${WAYLAND_SERVER_INCLUDE_DIRS}
    ${XKB_COMMON_INCLUDE_DIRS}
)

add_definitions(
    ${DRM_DEFINITIONS}
    ${EGL_DEFINITIONS}
    ${GBM_DEFINITIONS}
    ${PNG_DEFINITIONS}
    ${WAYLAND_CURSOR_DEFINITIONS}
    ${WAYLAND_SERVER_DEFINITIONS}
    ${XKB_COMMON_DEFINITIONS}
)

set(SOURCE_FILES
    buffer/buffer.c
    buffer/egl.c
    buffer/frame.c
    buffer/image.c
    buffer/raw_buffer.c
    cursor.c
    framebuffer_device.c
    keyboard.c
    module.c
    monitor.c
    monitor_mode.c
    wayland/abstract_shell_surface.c
    wayland/buffer.c
    wayland/client.c
    wayland/compositor.c
    wayland/keyboard.c
    wayland/pointer.c
    wayland/region.c
    wayland/seat.c
    wayland/shell.c
    wayland/shell_surface.c
    wayland/surface.c
    wayland/xdg_shell.c
    wayland/xdg_surface.c
)

add_library(compositor STATIC
    ${SOURCE_FILES}
)

target_link_libraries(compositor
    protocol
    objects
    logger

    ${DRM_LIBRARIES}
    ${EGL_LIBRARIES}
    ${GBM_LIBRARIES}
    ${PNG_LIBRARIES}
    ${WAYLAND_CURSOR_LIBRARIES}
    ${WAYLAND_SERVER_LIBRARIES}
    ${XKB_COMMON_LIBRARIES}
)

