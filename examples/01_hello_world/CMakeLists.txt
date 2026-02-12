# SPDX-FileCopyrightText: 2026 Juan Medina
# SPDX-License-Identifier: MIT

project(example_01_hello_world
        VERSION 0.1.0.0
        DESCRIPTION "example 01 - hello world"
        LANGUAGES CXX
)

file(GLOB_RECURSE APP_SOURCE_FILES "src/*.cpp")
file(GLOB_RECURSE COMMON_SOURCE_FILES "../src/*.cpp")

add_executable(${PROJECT_NAME} ${APP_SOURCE_FILES} ${COMMON_SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE lge)

# Place examples' runtime outputs in the build/examples folder
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples)

if (WIN32)
    target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:/SUBSYSTEM:CONSOLE>
            $<$<NOT:$<CONFIG:Debug>>:/SUBSYSTEM:WINDOWS>
    )
    target_compile_definitions(${PROJECT_NAME} PRIVATE
            WIN32_LEAN_AND_MEAN
            NOMINMAX
            NOUSER
            NOGDI
    )
endif ()


if (EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    set(CUSTOM_SHELL ${CMAKE_CURRENT_SOURCE_DIR}/../web/template.html)
    target_link_options(${PROJECT_NAME} PRIVATE
            "--shell-file=${CUSTOM_SHELL}"
            "-sUSE_GLFW=3"
            "-sALLOW_MEMORY_GROWTH=1"
            "--bind"
            "-sEXPORTED_RUNTIME_METHODS=['HEAPF32','HEAP32','HEAPU8','requestFullscreen']"
    )
    set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "hello_index")
    configure_file(../web/favicon.ico favicon.ico COPYONLY)
endif ()
