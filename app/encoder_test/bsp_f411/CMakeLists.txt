add_library(encoder_bsp STATIC
    f411_board.cc
)

target_include_directories(encoder_bsp PUBLIC 
  .
  ../
)

target_link_libraries(encoder_bsp PUBLIC
    driver
    core
)
