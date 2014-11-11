find_package(Threads REQUIRED)

set(TEST_SOURCES
    $<TARGET_OBJECTS:logger>
    $<TARGET_OBJECTS:values>
    ${PROJECT_SOURCE_DIR}/src/input/hotkey_dag.c
    ${PROJECT_SOURCE_DIR}/src/input/hotkey_event.c
    ${PROJECT_SOURCE_DIR}/src/objects/object.c
    ${PROJECT_SOURCE_DIR}/src/objects/set.c
    ${PROJECT_SOURCE_DIR}/src/objects/string.c
    ${PROJECT_SOURCE_DIR}/src/util/cleaner.c
    PARENT_SCOPE)

set(TEST_LIBRARIES
    ${UUID_LIBRARIES}
    ${libreset_LIBRARIES}

    ${ICU_OP_LIBRARIES}
    ${ICU_UC_LIBRARIES}
    PARENT_SCOPE)

set(TEST_INCLUDE_DIRS
    ${ICU_INCLUDE_DIRS}
    ${ICU_INCLUDE_DIRS}
    PARENT_SCOPE)

set(TEST_DEFINITIONS
    ${UUID_DEFINITIONS}
    ${libreset_DEFINITIONS}

    ${ICU_DEFINITIONS}
    ${ICU_DEFINITIONS}
    PARENT_SCOPE)

