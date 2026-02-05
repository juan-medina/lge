# Example: 01_hello_world
add_executable(example_01_hello_world main.cpp)

target_link_libraries(example_01_hello_world PRIVATE lge)

# Place examples' runtime outputs in the build/examples folder
set_target_properties(example_01_hello_world PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples)
