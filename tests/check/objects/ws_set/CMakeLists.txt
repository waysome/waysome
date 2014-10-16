find_package(Threads REQUIRED)
find_package(libreset REQUIRED)

set(TEST_SOURCES
    ${PROJECT_SOURCE_DIR}/src/logger/module.c
    ${PROJECT_SOURCE_DIR}/src/objects/object.c
    ${PROJECT_SOURCE_DIR}/src/objects/set.c
    ${PROJECT_SOURCE_DIR}/src/util/cleaner.c
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
