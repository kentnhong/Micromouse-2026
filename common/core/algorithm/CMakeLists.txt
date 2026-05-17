add_library(algorithm STATIC
    floodfill.cc
)

target_include_directories(algorithm PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/common/core/periph/irsensor
    ${CMAKE_SOURCE_DIR}/common/drivers/io
    ${CMAKE_SOURCE_DIR}/common/drivers/memory
)

