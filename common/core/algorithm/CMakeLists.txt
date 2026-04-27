add_library(algorithm STATIC
    floodfill.cc
)

target_include_directories(algorithm PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

