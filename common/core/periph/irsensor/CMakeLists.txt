add_library(irsensor INTERFACE)

target_sources(irsensor INTERFACE
    irsensor.cc
)

target_include_directories(irsensor INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(irsensor INTERFACE 
    driver
)