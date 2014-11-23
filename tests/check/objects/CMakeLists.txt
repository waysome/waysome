set(TEST_SUITES_OBJECTS
    ws_object
    ws_set
    ws_string
)

foreach(TEST IN LISTS TEST_SUITES_OBJECTS)
    set(TEST_EXE "check_${TEST}.test")
    add_executable(${TEST_EXE} EXCLUDE_FROM_ALL
        "${TEST}/test.c"
    )

    target_link_libraries(${TEST_EXE}
        objects
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
