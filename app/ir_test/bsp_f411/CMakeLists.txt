add_library(ir_bsp STATIC
    f411_board.cc
)

target_include_directories(ir_bsp PUBLIC 
  .
  ../
)

target_link_libraries(ir_bsp PUBLIC
    driver
    core
)