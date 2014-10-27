#
# Build the input submodule
#
set(SOURCE_FILES
    module.c
    input_device.c
    utils.c
)

add_library(input OBJECT
    ${SOURCE_FILES}
)

