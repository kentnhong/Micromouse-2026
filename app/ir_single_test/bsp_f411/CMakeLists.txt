add_library(ir_single_bsp STATIC
    f411_board.cc
)

target_include_directories(ir_single_bsp PUBLIC
    .
    ../
)

target_link_libraries(ir_single_bsp PUBLIC
    driver
)
