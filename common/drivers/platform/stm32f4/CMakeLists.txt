add_library(hal STATIC
    st_gpio.cc
    st_spi.cc
    st_i2c.cc
    st_pwm.cc
<<<<<<< HEAD
    st_sys_clk.cc
    st_usart.cc
    st_encoder.cc
    st_adc.cc
    st_dma.cc
=======
    st_adc.cc
>>>>>>> 0d93109 (first commit)
)

target_include_directories(hal PUBLIC
    .
    ..
    ${CMAKE_SOURCE_DIR}/mcu_support/stm32/f4xx
    ${CMAKE_SOURCE_DIR}/mcu_support/CMSIS/include
    ${CMAKE_SOURCE_DIR}/common/drivers/time
    ${CMAKE_SOURCE_DIR}/common/drivers/bus
    ${CMAKE_SOURCE_DIR}/common/drivers/io
<<<<<<< HEAD
    ${CMAKE_SOURCE_DIR}/common/drivers/memory
=======
>>>>>>> 0d93109 (first commit)
    ${CMAKE_SOURCE_DIR}
    )

target_link_libraries(hal PUBLIC
    st_f4_support
    core 
)