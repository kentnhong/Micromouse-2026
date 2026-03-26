add_library(drv_bsp
    f411_board.cc
)

target_include_directories(drv_bsp
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../bsp_f411
    PRIVATE ${CMAKE_SOURCE_DIR}/common/core/periph/drv8231
)

target_link_libraries(drv_bsp
    PUBLIC core drv8231
)