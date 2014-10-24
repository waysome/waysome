#
# Build the compositor submodule
#
set(SOURCE_FILES
    buffer/buffer.c
    buffer/frame.c
    buffer/image.c
    cursor.c
    framebuffer_device.c
    module.c
    monitor.c
    monitor_mode.c
    wayland/buffer.c
    wayland/compositor.c
    wayland/pointer.c
    wayland/seat.c
    wayland/shell.c
    wayland/shell_surface.c
    wayland/surface.c
)

add_library(compositor OBJECT
    ${SOURCE_FILES}
)

