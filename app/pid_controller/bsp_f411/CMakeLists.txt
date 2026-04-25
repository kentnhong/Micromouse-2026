add_library(pid_bsp f411_board.cc)

target_include_directories(pid_bsp
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../bsp_f411
    PRIVATE ${CMAKE_SOURCE_DIR}/common/drivers/platform/bno055
    PRIVATE ${CMAKE_SOURCE_DIR}/common/core/periph/drv8231
    PRIVATE ${CMAKE_SOURCE_DIR}/common/core/control
)

target_link_libraries(pid_bsp PUBLIC driver core bno055 drv8231 control motion)