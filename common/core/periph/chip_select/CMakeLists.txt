add_library(chip_select INTERFACE)

target_sources(chip_select INTERFACE
    gpio_cs.cc
)

target_include_directories(chip_select INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/common/drivers/io
)

target_link_libraries(chip_select INTERFACE
    driver
)