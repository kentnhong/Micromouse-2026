add_library(cli_bsp STATIC
  f411_board.cc
)

target_include_directories(cli_bsp PUBLIC 
  .
  ../
  ${CMAKE_SOURCE_DIR}/common/drivers/platform/stm32f4
  ${CMAKE_SOURCE_DIR}/common/drivers/time
  ${CMAKE_SOURCE_DIR}
)

target_link_libraries(cli_bsp PUBLIC
  driver
)

