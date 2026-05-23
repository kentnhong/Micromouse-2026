set(EXECUTABLE pid_controller)
set(LDF ${CMAKE_CURRENT_BINARY_DIR}/${EXECUTABLE}.ld)

set(PREPROCESS_DEFS
	-DDEF_FLASH_START_ADDR=0x8000000
	-DDEF_FLASH_SIZE=1024K
)

# Only add BSP for this target device
add_subdirectory_for(STM32F411 bsp_f411)

if (TARGET pid_bsp)

	add_executable_for(${TARGET_DEVICE} ${EXECUTABLE} ${LDF}
		main.cc
		${STARTUP_FILE}
	)

	target_include_directories_for(${TARGET_DEVICE} ${EXECUTABLE}
		PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
		PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/bsp_f411
	)

	target_link_libraries_for(${TARGET_DEVICE} ${EXECUTABLE} PRIVATE
		pid_bsp
		motion
	)

	target_preprocess_for(${TARGET_DEVICE} ${EXECUTABLE} ${LINKER_SCRIPT} ${LDF} ${PREPROCESS_DEFS})

	target_compile_options(${EXECUTABLE} PRIVATE -g3)

	add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy
			$<TARGET_FILE:${EXECUTABLE}>
			${CMAKE_CURRENT_BINARY_DIR}/${EXECUTABLE}.elf
	)

endif()

if("${TARGET_DEVICE}" STREQUAL "NATIVE")
    # No native test subdirectory exists for this app.
endif()
