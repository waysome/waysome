find_package(Check)

include_directories(
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/tests/check
    ${CHECK_INCLUDE_DIRS}
)

#
# This variable will hold all the tests
#
set (TEST_SUITES
    action
    command
    compositor
    connection
    logger
    objects
    serialize
    session
    storage
    util
    values
)

#
#
#
if (${CHECK_FOUND})
    foreach(TEST_SUITE IN LISTS TEST_SUITES)
        set(TEST_EXE "check_${TEST_SUITE}.test")
        add_executable(${TEST_EXE} EXCLUDE_FROM_ALL
            "${TEST_SUITE}/${TEST_SUITE}_tests.c"
        )

        target_link_libraries(${TEST_EXE}
            ${CHECK_LIBRARY}
        )

        add_test("build_${TEST_EXE}" "${CMAKE_COMMAND}"
            --build ${CMAKE_BINARY_DIR}
            --target "${TEST_EXE}"
        )
        add_test("${TEST_EXE}" ${TEST_EXE})
        set_tests_properties(${TEST_EXE} PROPERTIES DEPENDS
            "build_${TEST_EXE}"
        )
    endforeach()
else()
    message("Check framework not found")
endif()


