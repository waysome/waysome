#
# Build the serialize submodule
#
set(SOURCE_FILES
    deserializer.c
    json/common.c
    json/deserializer.c
    json/deserializer_state.c
    json/string_jump_state.c
    serializer.c
)

add_library(serialize OBJECT
    ${SOURCE_FILES}
)

