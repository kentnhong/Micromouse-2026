add_library(pid STATIC pid.cc)

target_include_directories(pid PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/common/core/math
    ${CMAKE_SOURCE_DIR}/common/core/periph/bno055
    ${CMAKE_SOURCE_DIR}/common/drivers/time
    ${CMAKE_SOURCE_DIR}/common/drivers/bus
    ${CMAKE_SOURCE_DIR}/common/core/control/profile
    ${CMAKE_SOURCE_DIR}/common/core/control/sample
)