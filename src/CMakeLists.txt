
add_executable(TeenyWare main.c)
target_compile_options(TeenyWare PRIVATE -Wall -Werror -O0 -g)
target_link_libraries(TeenyWare PRIVATE raylib)

# Copy resources to bin directory during build
add_custom_command(TARGET TeenyWare POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/resources
    ${CMAKE_BINARY_DIR}/bin/resources
    COMMENT "Copying resources to build directory"
)
