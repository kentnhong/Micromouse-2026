set(EXECUTABLE trap_test)
set(LDF ${CMAKE_CURRENT_BINARY_DIR}/${EXECUTABLE}.ld)

set(PREPROCESS_DEFS
    -DDEF_FLASH_START_ADDR=0x8000000
    -DDEF_FLASH_SIZE=1024K
)

add_subdirectory_for(STM32F411 bsp_f411)

if (TARGET trap_bsp)

    add_executable_for(${TARGET_DEVICE} ${EXECUTABLE} ${LDF}
        main.cc
        ${STARTUP_FILE}
    )

    target_include_directories_for(${TARGET_DEVICE} ${EXECUTABLE}
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
        PRIVATE ${CMAKE_SOURCE_DIR}/common/core/periph/drv8231
        PRIVATE ${CMAKE_SOURCE_DIR}/common/core/control/pid
        PRIVATE ${CMAKE_SOURCE_DIR}/common/core/control/profile
        PRIVATE ${CMAKE_SOURCE_DIR}/common/core/control/sample
        PRIVATE ${CMAKE_SOURCE_DIR}/common/core/math
        PRIVATE ${CMAKE_SOURCE_DIR}/common/drivers/io
        PRIVATE ${CMAKE_SOURCE_DIR}/common/drivers/time
    )

    target_link_libraries_for(${TARGET_DEVICE} ${EXECUTABLE} PRIVATE
        trap_bsp drv8231 pid profile sample
    )

    target_preprocess_for(${TARGET_DEVICE} ${EXECUTABLE} ${LINKER_SCRIPT} ${LDF} ${PREPROCESS_DEFS})

endif()
