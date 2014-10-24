#
# Build the objects submodule
#
set(SOURCE_FILES
    array.c
    message/error_reply.c
    message/message.c
    message/reply.c
    message/transaction.c
    message/value_reply.c
    object.c
    set.c
    string.c
    wayland_obj.c
)

add_library(objects OBJECT
    ${SOURCE_FILES}
)

