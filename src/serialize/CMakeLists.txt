#
# Build the serialize submodule
#

include_directories(
    ${YAJL_INCLUDE_DIRS}
)

add_definitions(
    ${YAJL_DEFINITIONS}
)

set(SOURCE_FILES
    deserializer.c
    json/deserializer.c
    json/deserializer_callbacks.c
    json/serializer.c
    json/serializer_state.c
    serializer.c
)

add_library(serialize STATIC
    ${SOURCE_FILES}
)

target_link_libraries(serialize
    objects
    command

    ${YAJL_LIBRARIES}
)

