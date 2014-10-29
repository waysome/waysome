#
# Build the compositor submodule
#
set(SOURCE_FILES
    buffer/buffer.c
    buffer/frame.c
    buffer/image.c
    cursor.c
    framebuffer_device.c
    keyboard.c
    module.c
    monitor.c
    monitor_mode.c
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

add_library(compositor OBJECT
    ${SOURCE_FILES}
)

