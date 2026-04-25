add_library(sample STATIC enc_sample.cc)

target_include_directories(sample PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/common/core/math
    ${CMAKE_SOURCE_DIR}/common/drivers/io
)
