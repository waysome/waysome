#
# Build the connection submodule
#
set(SOURCE_FILES
    connbuf.c
    connector.c
)

add_library(connection OBJECT
    ${SOURCE_FILES}
)

