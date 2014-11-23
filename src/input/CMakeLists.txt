#
# Build the input submodule
#

include_directories(
    ${EVDEV_INCLUDE_DIRS}
)

add_definitions(
    ${EVDEV_DEFINITIONS}
)

set(SOURCE_FILES
    hotkey_dag.c
    hotkey_event.c
    hotkeys.c
    input_device.c
    module.c
    utils.c
)

add_library(input STATIC
    ${SOURCE_FILES}
)

target_link_libraries(input
    objects

    ${EVDEV_LIBRARIES}
)

