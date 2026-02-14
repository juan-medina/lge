# SPDX-FileCopyrightText: 2026 Juan Medina
# SPDX-License-Identifier: MIT

project(example_01_hello_world
        VERSION 0.1.0.0
        DESCRIPTION "example 01 - hello world"
        LANGUAGES CXX
)

file(GLOB_RECURSE APP_SOURCE_FILES "src/*.cpp")
file(GLOB_RECURSE COMMON_SOURCE_FILES "../src/*.cpp")

include(${CMAKE_SOURCE_DIR}/cmake/LGEApplication.cmake)

lge_add_application(${PROJECT_NAME}
        SOURCES ${APP_SOURCE_FILES} ${COMMON_SOURCE_FILES}
        EMSCRIPTEN_SHELL ${CMAKE_CURRENT_SOURCE_DIR}/../web/template.html
        EMSCRIPTEN_ASSETS ${CMAKE_CURRENT_SOURCE_DIR}/../web/favicon.ico
)
