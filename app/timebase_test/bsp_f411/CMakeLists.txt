add_library(timebase_bsp STATIC
    f411_board.cc
)

target_include_directories(timebase_bsp PUBLIC 
  .
  ../
)

target_link_libraries(timebase_bsp PUBLIC
    driver
    core
)
