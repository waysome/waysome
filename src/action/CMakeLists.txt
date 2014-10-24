#
# Build the action submodule
#
set(SOURCE_FILES
    manager.c
    processor.c
    processor_stack.c
)

add_library(action OBJECT
    ${SOURCE_FILES}
)

