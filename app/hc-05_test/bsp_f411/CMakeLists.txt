add_library(bluetooth_app_bsp STATIC
    f411_board.cc
)

target_include_directories(bluetooth_app_bsp PUBLIC 
  .
  ../
)

target_link_libraries(bluetooth_app_bsp PUBLIC
    driver
)

