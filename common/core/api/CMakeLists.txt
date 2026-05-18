add_library(api STATIC
    motioncontroller.cc
)

target_include_directories(api PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(api PUBLIC
    irsensor
    drv8231
)
