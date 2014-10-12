find_package(Threads REQUIRED)
find_package(libreset REQUIRED)

set(TEST_SOURCES
    ${PROJECT_SOURCE_DIR}/src/objects/object.c
    ${PROJECT_SOURCE_DIR}/src/objects/set.c
    PARENT_SCOPE)

set(TEST_LIBRARIES
    ${Threads_LIBRARIES}
    ${libreset_LIBRARIES}
    PARENT_SCOPE)

set(TEST_INCLUDE_DIRS
    ${Threads_LIBRARIES}
    ${libreset_INCLUDE_DIRS}
    PARENT_SCOPE)

set(TEST_DEFINITIONS
    ${Threads_LIBRARIES}
    ${libreset_DEFINITIONS}
    PARENT_SCOPE)
