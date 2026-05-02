if("${TARGET_DEVICE}" STREQUAL "NATIVE")
    add_executable(pid_tests
        test.cc
    )

    target_link_libraries(pid_tests
        PRIVATE
            GTest::gtest_main
            pid
            drv8231
    )

    add_test(NAME pid_tests COMMAND pid_tests)
endif()