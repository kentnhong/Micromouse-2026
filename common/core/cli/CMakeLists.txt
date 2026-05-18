add_library(cli STATIC
    cli.cc
)

target_include_directories(cli PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

