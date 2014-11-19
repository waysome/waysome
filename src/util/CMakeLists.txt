#
# Build the util submodule
#
set(SOURCE_FILES
    cleaner.c
    socket.c
    wayland.c
    exec.c
)

add_library(util OBJECT
    ${SOURCE_FILES}
)

