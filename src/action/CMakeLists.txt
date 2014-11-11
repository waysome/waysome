#
# Build the action submodule
#
set(SOURCE_FILES
    context.c
    manager.c
    processor.c
    processor_stack.c
)

add_library(action OBJECT
    ${SOURCE_FILES}
)

