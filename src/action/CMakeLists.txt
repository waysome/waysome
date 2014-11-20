#
# Build the action submodule
#
set(SOURCE_FILES
    commands.c
    manager.c
    processor.c
    processor_stack.c
)

add_library(action STATIC
    ${SOURCE_FILES}
)

target_link_libraries(action
    command
    values
)

