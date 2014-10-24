#
# Build the logger submodule
#
set(SOURCE_FILES
    module.c
)

add_library(logger OBJECT
    ${SOURCE_FILES}
)

