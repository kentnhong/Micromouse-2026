add_library(pid STATIC
    pid.cc
)

add_subdirectory(helpers)

target_include_directories(pid PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/common/core/math
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
    ${CMAKE_SOURCE_DIR}/common/core/periph/bno055
    ${CMAKE_SOURCE_DIR}/common/drivers/time
    ${CMAKE_SOURCE_DIR}/common/drivers/bus
    ${CMAKE_SOURCE_DIR}/common/core/control/helpers
)

<<<<<<< HEAD
target_link_libraries(pid PUBLIC helpers)
=======
)
>>>>>>> 2e41c8d (commit for rebase)
=======
=======
>>>>>>> 4bebdbe (PID control adding init)
)
>>>>>>> efd260e (PID control adding init)
=======
)
>>>>>>> 4895d09 (PID structure + ideas)
=======
    ${CMAKE_SOURCE_DIR}/common/core/periph/bno055
    ${CMAKE_SOURCE_DIR}/common/drivers/time
    ${CMAKE_SOURCE_DIR}/common/drivers/bus
)
>>>>>>> 306c6c6 (Change the math to per wheel (l/r); so we don't deal with yaw, this is only speed of each wheel dictate the direction)
=======
target_link_libraries(pid PUBLIC helpers)
>>>>>>> 662638f (add elapsed time for ensure the PID controller updates are based on accurate)
