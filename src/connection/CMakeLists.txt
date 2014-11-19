#
# Build the connection submodule
#
set(SOURCE_FILES
    config_file.c
    connbuf.c
    connector.c
    processor.c
)

add_library(connection OBJECT
    ${SOURCE_FILES}
)

