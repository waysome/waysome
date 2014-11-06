find_package(Threads REQUIRED)

set(TEST_SOURCES
    ${PROJECT_SOURCE_DIR}/src/logger/module.c
    ${PROJECT_SOURCE_DIR}/src/objects/object.c
    ${PROJECT_SOURCE_DIR}/src/objects/string.c
    ${PROJECT_SOURCE_DIR}/src/util/cleaner.c
    ${PROJECT_SOURCE_DIR}/src/values/int.c
    ${PROJECT_SOURCE_DIR}/src/values/object_id.c
    ${PROJECT_SOURCE_DIR}/src/values/string.c
    ${PROJECT_SOURCE_DIR}/src/values/value.c
    PARENT_SCOPE)

set(TEST_LIBRARIES
    ${Threads_LIBRARIES}
    ${UUID_LIBRARIES}
    ${ICU_OP_LIBRARIES}
    ${ICU_UC_LIBRARIES}
    PARENT_SCOPE)

set(TEST_INCLUDE_DIRS
    ${Threads_LIBRARIES}
    ${UUID_INCLUDE_DIRS}
    ${ICU_OP_INCLUDE_DIRS}
    ${ICU_UC_INCLUDE_DIRS}
    PARENT_SCOPE)

set(TEST_DEFINITIONS
    ${Threads_LIBRARIES}
    ${UUID_DEFINITIONS}
    ${ICU_OP_DEFINITIONS}
    ${ICU_UC_DEFINITIONS}
    PARENT_SCOPE)


