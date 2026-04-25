add_library(control INTERFACE)

add_subdirectory(profile)
add_subdirectory(sample)
add_subdirectory(pid)
add_subdirectory(motion)

target_link_libraries(control INTERFACE
    profile
    sample
    pid
    motion
)
