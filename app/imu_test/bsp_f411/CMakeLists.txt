add_library(imu_bsp f411_board.cc)

target_include_directories(imu_bsp
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../bsp_f411
    PRIVATE ${CMAKE_SOURCE_DIR}/common/drivers/platform/bno055
)

target_link_libraries(imu_bsp PUBLIC core bno055)