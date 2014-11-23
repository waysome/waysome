#
# Build the logger submodule
#
set(SOURCE_FILES
    module.c
)

add_library(logger STATIC
    ${SOURCE_FILES}
)

target_link_libraries(logger
    util
)

