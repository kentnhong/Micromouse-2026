add_library(nav_bsp f411_board.cc)

target_include_directories(nav_bsp
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../bsp_f411
    PRIVATE ${CMAKE_SOURCE_DIR}/common/core/navigation
    PRIVATE ${CMAKE_SOURCE_DIR}/common/core/api
    PRIVATE ${CMAKE_SOURCE_DIR}/common/core/periph/irsensor
    PRIVATE ${CMAKE_SOURCE_DIR}/common/core/periph/drv8231
    PRIVATE ${CMAKE_SOURCE_DIR}/common/core/periph/encoder
    PRIVATE ${CMAKE_SOURCE_DIR}/common/drivers/time
    PRIVATE ${CMAKE_SOURCE_DIR}/common/drivers/io
    PRIVATE ${CMAKE_SOURCE_DIR}/common/drivers/bus
)

target_link_libraries(nav_bsp PUBLIC core navigation api irsensor drv8231 encoder)
