set(TEST_SUITES_SERIALIZER
    json_deserializer
    json_serializer
)

foreach(TEST IN LISTS TEST_SUITES_SERIALIZER)
    add_subdirectory("${TEST}")

    if (DEFINED TEST_INCLUDE_DIRS)
        include_directories(${TEST_INCLUDE_DIRS})
    endif()
    if (DEFINED TEST_DEFINITIONS)
        add_definitions(${TEST_DEFINITIONS})
    endif()

    set(TEST_EXE "check_${TEST}.test")
    add_executable(${TEST_EXE} EXCLUDE_FROM_ALL
        "${TEST}/test.c"
        ${TEST_SOURCES}
    )

    target_link_libraries(${TEST_EXE}
        ${CHECK_LIBRARY}
        ${TEST_LIBRARIES}
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

