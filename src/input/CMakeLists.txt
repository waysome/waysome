#
# Build the input submodule
#
set(SOURCE_FILES
    hotkey_dag.c
    hotkey_event.c
    input_device.c
    module.c
    utils.c
)

add_library(input OBJECT
    ${SOURCE_FILES}
)

