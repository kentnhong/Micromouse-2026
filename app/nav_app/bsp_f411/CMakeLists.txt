add_library(nav_bsp STATIC
    f411_board.cc
)

target_include_directories(nav_bsp
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/../
    PRIVATE ${CMAKE_SOURCE_DIR}/common/core/api
    PRIVATE ${CMAKE_SOURCE_DIR}/common/core/navigation
    PRIVATE ${CMAKE_SOURCE_DIR}/common/core/periph/irsensor
    PRIVATE ${CMAKE_SOURCE_DIR}/common/core/periph/drv8231
    PRIVATE ${CMAKE_SOURCE_DIR}/common/core/control/pid
    PRIVATE ${CMAKE_SOURCE_DIR}/common/core/control/profile
    PRIVATE ${CMAKE_SOURCE_DIR}/common/core/control/sample
    PRIVATE ${CMAKE_SOURCE_DIR}/common/core/math
    PRIVATE ${CMAKE_SOURCE_DIR}/common/drivers/io
    PRIVATE ${CMAKE_SOURCE_DIR}/common/drivers/platform/stm32f4
    PRIVATE ${CMAKE_SOURCE_DIR}/common/drivers/time
)

target_link_libraries(nav_bsp PUBLIC
    core
    navigation
    api
    irsensor
    drv8231
    pid
    profile
    sample
    driver
)
