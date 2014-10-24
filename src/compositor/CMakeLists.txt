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
    wayland/seat.c
    wayland/surface.c
    wayland/wayland_compositor.c
    wayland/wayland_pointer.c
    wayland/wayland_shell.c
    wayland/wayland_shell_surface.c
)

add_library(compositor OBJECT
    ${SOURCE_FILES}
)

