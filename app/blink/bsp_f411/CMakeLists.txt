add_library(blink_bsp STATIC
  f411_board.cc
  hal_timebase.c
)

target_include_directories(blink_bsp PUBLIC 
  .
  ../
  ${CMAKE_SOURCE_DIR}/common/drivers/platform/stm32f4
  ${CMAKE_SOURCE_DIR}/common/drivers/time
  ${CMAKE_SOURCE_DIR}
)

target_link_libraries(blink_bsp PUBLIC
  driver
)

