add_library(cli_bsp STATIC
    f411_board.cc
)

target_include_directories(cli_bsp PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    ${CMAKE_SOURCE_DIR}/common/core/api
    ${CMAKE_SOURCE_DIR}/common/core/periph/irsensor
    ${CMAKE_SOURCE_DIR}/common/core/periph/drv8231
    ${CMAKE_SOURCE_DIR}/common/core/control/pid
    ${CMAKE_SOURCE_DIR}/common/core/control/profile
    ${CMAKE_SOURCE_DIR}/common/core/control/sample
    ${CMAKE_SOURCE_DIR}/common/core/math
    ${CMAKE_SOURCE_DIR}/common/drivers/io
    ${CMAKE_SOURCE_DIR}/common/drivers/platform/stm32f4
    ${CMAKE_SOURCE_DIR}/common/drivers/time
)

target_link_libraries(cli_bsp PUBLIC
    core
    api
    irsensor
    drv8231
    pid
    profile
    sample
    driver
)
