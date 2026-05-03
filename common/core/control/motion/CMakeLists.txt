add_library(motion STATIC motion.cc)

target_include_directories(motion PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/app #This is need to be chance once we get the CLI app
    ${CMAKE_SOURCE_DIR}/common/core/math
    ${CMAKE_SOURCE_DIR}/common/core/periph/bno055
    ${CMAKE_SOURCE_DIR}/common/core/periph/drv8231
    ${CMAKE_SOURCE_DIR}/common/drivers/io
    ${CMAKE_SOURCE_DIR}/common/drivers/time
    ${CMAKE_SOURCE_DIR}/common/drivers/bus
    ${CMAKE_SOURCE_DIR}/common/core/control/sample
    ${CMAKE_SOURCE_DIR}/common/core/control/pid
)

target_link_libraries(motion PUBLIC
    sample
    pid
)
