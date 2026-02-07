add_library( w25q128 INTERFACE)

target_sources(w25q128 INTERFACE
    w25q.cc
)

target_include_directories(w25q128 INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/common/drivers/bus
)

target_link_libraries(w25q128 INTERFACE
    driver
    chip_select
)