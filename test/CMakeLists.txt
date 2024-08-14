if(BUILD_TESTING)
    set(FILE thread_safe_queue_test)
    add_executable(${FILE} ${FILE}.cpp)

    target_link_libraries(${FILE}
        PRIVATE GTest::gtest_main
        PUBLIC thread_safe_queue
    )

    include(GoogleTest)
    gtest_discover_tests(${FILE})
endif()