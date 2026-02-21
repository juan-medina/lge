# SPDX-FileCopyrightText: 2026 Juan Medina
# SPDX-License-Identifier: MIT

project(lge_tests
        VERSION 0.1.0.0
        DESCRIPTION "lge engine tests"
        LANGUAGES CXX
)

file(GLOB_RECURSE TEST_SOURCE_FILES "src/*.cpp")

add_executable(${PROJECT_NAME} ${TEST_SOURCE_FILES})

target_compile_definitions(${PROJECT_NAME} PRIVATE SPDLOG_USE_STD_FORMAT)

target_include_directories(${PROJECT_NAME}
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        lge
        Catch2::Catch2WithMain
)

include(CTest)
include(Catch)
catch_discover_tests(${PROJECT_NAME})