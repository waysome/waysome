#
# Build the connection submodule
#

include_directories(
    ${EV_INCLUDE_DIRS}
)

add_definitions(
    ${EV_DEFINITIONS}
)

set(SOURCE_FILES
    config_file.c
    connbuf.c
    connector.c
    manager.c
    processor.c
)

add_library(connection STATIC
    ${SOURCE_FILES}
)

target_link_libraries(connection
    action
    serialize

    ${EV_LIBRARIES}
)

