add_library(profile STATIC trapezoidal.cc)

target_include_directories(profile PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/common/drivers/time
    ${CMAKE_SOURCE_DIR}/common/core/math
)