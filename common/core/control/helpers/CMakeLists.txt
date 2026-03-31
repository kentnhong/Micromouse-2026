
add_library(helpers STATIC elapsed.cc)

target_include_directories(helpers PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/common/drivers/time
<<<<<<< HEAD
<<<<<<< HEAD
    ${CMAKE_SOURCE_DIR}/common/core/math
=======
>>>>>>> 662638f (add elapsed time for ensure the PID controller updates are based on accurate)
=======
    ${CMAKE_SOURCE_DIR}/common/core/math
>>>>>>> 383affe (Trapezoidal; Generate dynamic speed setpoints based on progress)
)

