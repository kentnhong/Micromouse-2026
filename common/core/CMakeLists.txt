add_subdirectory(periph)
add_subdirectory(utils)
add_subdirectory(control)

# Make core consumers also get utils and chip_select by adding them to the INTERFACE core target
# `core` is defined in the parent `common/CMakeLists.txt` as an INTERFACE target.
if (TARGET core)
<<<<<<< HEAD
	target_link_libraries(core INTERFACE utils chip_select bno055 w25q128 pid irsensor)
=======
	target_link_libraries(core INTERFACE utils chip_select bno055 w25q128 )
>>>>>>> a6f4b58 (PID controller main loop clean up & sorting folder)
endif()
