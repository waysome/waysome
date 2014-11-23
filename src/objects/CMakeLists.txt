#
# Build the objects submodule
#

include_directories(
    ${ICU_OP_INCLUDE_DIRS}
    ${ICU_UC_INCLUDE_DIRS}
    ${libreset_INCLUDE_DIRS}
)

add_definitions(
    ${ICU_OP_DEFINITIONS}
    ${ICU_UC_DEFINITIONS}
    ${libreset_DEFINITIONS}
)

set(SOURCE_FILES
    message/error_reply.c
    message/event.c
    message/message.c
    message/reply.c
    message/transaction.c
    message/value_reply.c
    named.c
    object.c
    set.c
    string.c
    wayland_obj.c
)

add_library(objects STATIC
    ${SOURCE_FILES}
)

target_link_libraries(objects
    command
    values
    logger
    util

    ${ICU_OP_LIBRARIES}
    ${ICU_UC_LIBRARIES}
    ${libreset_LIBRARIES}
)

