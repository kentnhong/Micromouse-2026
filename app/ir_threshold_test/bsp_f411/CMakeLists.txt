add_library(ir_threshold_bsp STATIC
    f411_board.cc
)

target_include_directories(ir_threshold_bsp PUBLIC
    .
    ../
)

target_link_libraries(ir_threshold_bsp PUBLIC
    driver
    core
)
