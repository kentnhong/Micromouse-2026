add_library(adc_bsp STATIC
    f411_board.cc
)

target_include_directories(adc_bsp PUBLIC 
  .
  ../
)

target_link_libraries(adc_bsp PUBLIC
    #st_l4_support #grabed in driver i think
  #        app
#        core
    driver
)

