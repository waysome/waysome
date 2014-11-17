#
# Build the serialize submodule
#
set(SOURCE_FILES
    deserializer.c
    json/deserializer.c
    json/deserializer_callbacks.c
    json/serializer.c
    json/serializer_state.c
    json/string_jump_state.c
    serializer.c
)

add_library(serialize OBJECT
    ${SOURCE_FILES}
)

