if ("${TARGET_DEVICE}" MATCHES "NATIVE")
    add_executable(mms_sim
        main.cc
        sim/mms.cc
        ${CMAKE_SOURCE_DIR}/common/core/algorithm/floodfill.cc
    )

    target_include_directories(mms_sim PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/common/core/algorithm
    )

endif()
