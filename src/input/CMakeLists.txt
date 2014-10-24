#
# Build the input submodule
#
set(SOURCE_FILES
    module.c
)

add_library(input OBJECT
    ${SOURCE_FILES}
)

