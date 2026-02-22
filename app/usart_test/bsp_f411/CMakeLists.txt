add_library(usart_bsp STATIC
    f411_board.cc
)

target_include_directories(usart_bsp PUBLIC 
  .
  ../
)

target_link_libraries(usart_bsp PUBLIC
    driver
)

