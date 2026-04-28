add_library(nav_bsp f411_board.cc)

target_include_directories(nav_bsp
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../bsp_f411
    PRIVATE ${CMAKE_SOURCE_DIR}/common/drivers/platform/bno055
    PRIVATE ${CMAKE_SOURCE_DIR}/common/core/navigation
)

target_link_libraries(nav_bsp PUBLIC core bno055 navigation)