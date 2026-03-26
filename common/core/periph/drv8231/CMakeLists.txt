add_library(drv8231 INTERFACE)

target_sources(drv8231 INTERFACE
    drv8231.cc
)

target_include_directories(drv8231 INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(drv8231 INTERFACE 
    driver
)