find_package(Threads REQUIRED)
find_package(libreset REQUIRED)

set(TEST_SOURCES
    ${PROJECT_SOURCE_DIR}/src/logger/module.c
    ${PROJECT_SOURCE_DIR}/src/objects/object.c
    ${PROJECT_SOURCE_DIR}/src/objects/set.c
    ${PROJECT_SOURCE_DIR}/src/objects/string.c
    ${PROJECT_SOURCE_DIR}/src/util/cleaner.c
    ${PROJECT_SOURCE_DIR}/src/values/int.c
    ${PROJECT_SOURCE_DIR}/src/values/object_id.c
    ${PROJECT_SOURCE_DIR}/src/values/string.c
    ${PROJECT_SOURCE_DIR}/src/values/value.c
    PARENT_SCOPE)

set(TEST_LIBRARIES
    ${Threads_LIBRARIES}
    ${libreset_LIBRARIES}
    ${UUID_LIBRARIES}
    ${ICU_OP_LIBRARIES}
    ${ICU_UC_LIBRARIES}
    PARENT_SCOPE)

set(TEST_INCLUDE_DIRS
    ${Threads_LIBRARIES}
    ${libreset_INCLUDE_DIRS}
    ${UUID_INCLUDE_DIRS}
    ${ICU_OP_INCLUDE_DIRS}
    ${ICU_UC_INCLUDE_DIRS}
    PARENT_SCOPE)

set(TEST_DEFINITIONS
    ${Threads_LIBRARIES}
    ${libreset_DEFINITIONS}
    ${UUID_DEFINITIONS}
    ${ICU_OP_DEFINITIONS}
    ${ICU_UC_DEFINITIONS}
    PARENT_SCOPE)
