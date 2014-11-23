#
# Build the values submodule
#
set(SOURCE_FILES
    bool.c
    int.c
    nil.c
    object_id.c
    set.c
    string.c
    union.c
    utils.c
    value.c
    value_named.c
    value_type.c
)

add_library(values STATIC
    ${SOURCE_FILES}
)

target_link_libraries(values
    objects
)

