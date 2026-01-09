add_library(hal STATIC
    st_gpio.cc
    st_spi.cc
    st_i2c.cc
)

target_include_directories(hal PUBLIC
    .
    ..
    ${CMAKE_SOURCE_DIR}/mcu_support/stm32/f4xx
    ${CMAKE_SOURCE_DIR}/mcu_support/CMSIS/include
    ${CMAKE_SOURCE_DIR}/common/drivers/time
    ${CMAKE_SOURCE_DIR}/common/drivers/bus
    ${CMAKE_SOURCE_DIR}
    )

target_link_libraries(hal PUBLIC
    st_f4_support
    core 
)