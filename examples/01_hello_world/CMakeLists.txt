# SPDX-FileCopyrightText: 2026 Juan Medina
# SPDX-License-Identifier: MIT

project(example_01_hello_world
        VERSION 0.1.0.0
        DESCRIPTION "example 01 - hello world"
        LANGUAGES CXX
)


file(GLOB_RECURSE APP_SOURCE_FILES "src/*.cpp")


add_executable(${PROJECT_NAME} ${APP_SOURCE_FILES})

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
    )
endif ()