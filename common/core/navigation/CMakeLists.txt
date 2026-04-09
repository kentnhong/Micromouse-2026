add_library(navigation STATIC
    nav.cc
)

target_include_directories(navigation PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/common/core/math
    ${CMAKE_SOURCE_DIR}/common/core/periph/bno055
    ${CMAKE_SOURCE_DIR}/common/drivers/time
)