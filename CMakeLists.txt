# SPDX-FileCopyrightText: 2026 Juan Medina
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.13)

project(lge
        VERSION 0.1.0.0
        DESCRIPTION "little game engine"
        LANGUAGES CXX
)

# Auto-detect platform for Emscripten builds
if (CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    if (NOT DEFINED PLATFORM)
        set(PLATFORM "Web" CACHE STRING "Target platform")
    endif ()
endif ()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE APP_PUBLIC_HEADERS "include/lge/*.hpp")
file(GLOB_RECURSE APP_SOURCE_FILES "src/lge/*.cpp")

set(EMSCRIPTEN FALSE)
if (CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    set(EMSCRIPTEN TRUE)
endif ()

add_library(${PROJECT_NAME} STATIC ${APP_SOURCE_FILES})

if (APP_PUBLIC_HEADERS)
    target_sources(${PROJECT_NAME} PUBLIC ${APP_PUBLIC_HEADERS})
endif ()

target_include_directories(${PROJECT_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

if (WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
            WIN32_LEAN_AND_MEAN
            NOMINMAX
            NOUSER
            NOGDI
    )
endif ()
