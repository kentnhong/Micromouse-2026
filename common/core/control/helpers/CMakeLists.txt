add_library(helpers STATIC elapsed.cc trapezoidal.cc)

target_include_directories(helpers PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/common/drivers/time
    ${CMAKE_SOURCE_DIR}/common/core/math
)