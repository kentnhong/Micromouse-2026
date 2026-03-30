add_library(pid STATIC
    pid.cc
)

target_include_directories(pid PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/common/core/math
)