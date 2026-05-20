add_library(hal2 STATIC
    st_enc_hal.cc
    st_pwm_hal.cc
)


target_include_directories(hal2 PUBLIC
    .
    ..
    ${CMAKE_SOURCE_DIR}/mcu_support/stm32/f4xx
    ${CMAKE_SOURCE_DIR}/mcu_support/CMSIS/include
    ${CMAKE_SOURCE_DIR}/common/drivers/time
    ${CMAKE_SOURCE_DIR}/common/drivers/bus
    ${CMAKE_SOURCE_DIR}/common/drivers/io
    ${CMAKE_SOURCE_DIR}/common/drivers/memory
    ${CMAKE_SOURCE_DIR}
    )

target_link_libraries(hal2 PUBLIC
    st_f4_support
    core 
)