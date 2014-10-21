#
# Build the util submodule
#
set(SOURCE_FILES
    cleaner.c
    config_file.c
    wayland.c
)

add_library(util OBJECT
    ${SOURCE_FILES}
)

