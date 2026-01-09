if (TARGET_DEVICE MATCHES "^(STM32F4)[0-9]+")
    add_subdirectory(platform/stm32f4)
endif()

add_subdirectory(time)

add_library(driver INTERFACE)

target_include_directories(driver INTERFACE
    bus
    io 
)

target_link_libraries(driver INTERFACE
    hal
    
)