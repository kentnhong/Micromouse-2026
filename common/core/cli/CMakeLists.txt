add_library(cli_core STATIC
    cli.cc
)

target_include_directories(cli_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(cli_core PUBLIC
    navigation
    api
    driver
)
