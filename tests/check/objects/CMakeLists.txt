find_package(Threads REQUIRED)

set(TEST_SOURCES
    ${PROJECT_SOURCE_DIR}/src/objects/object.c
    PARENT_SCOPE)

set(TEST_LIBRARIES
    ${Threads_LIBRARIES}
    PARENT_SCOPE)

set(TEST_INCLUDE_DIRS
    ${Threads_LIBRARIES}
    PARENT_SCOPE)

set(TEST_DEFINITIONS
    ${Threads_LIBRARIES}
    PARENT_SCOPE)
