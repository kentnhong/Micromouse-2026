add_library(bsp_f411 f411_board.cc)

target_include_directories(bsp_f411
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../bsp_f411
    PRIVATE ${CMAKE_SOURCE_DIR}/common/drivers/platform/bno055
)

target_link_libraries(bsp_f411 PUBLIC core bno055)