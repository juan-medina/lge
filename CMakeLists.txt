# SPDX-FileCopyrightText: 2026 Juan Medina
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.28)

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

file(GLOB_RECURSE LIB_PUBLIC_HEADERS "include/lge/*.hpp")
file(GLOB_RECURSE LIB_SOURCE_FILES "src/lge/*.cpp")

set(EMSCRIPTEN FALSE)
if (CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    set(EMSCRIPTEN TRUE)
endif ()

add_library(${PROJECT_NAME} STATIC ${LIB_SOURCE_FILES})

if (LIB_PUBLIC_HEADERS)
    target_sources(${PROJECT_NAME} PUBLIC ${LIB_PUBLIC_HEADERS})
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

if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else ()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
endif ()

# spdlog
add_subdirectory(external/spdlog)
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog_header_only)
target_compile_definitions(${PROJECT_NAME} PUBLIC SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)
target_compile_definitions(${PROJECT_NAME} PUBLIC SPDLOG_USE_STD_FORMAT)

# raylib
add_subdirectory(external/raylib)
target_link_libraries(${PROJECT_NAME} PUBLIC raylib)
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/raylib/src)

# entt
add_subdirectory(external/entt)
target_link_libraries(${PROJECT_NAME} PUBLIC EnTT::EnTT)
target_include_directories(lge SYSTEM PUBLIC external/entt/src/entt)


# glm
add_subdirectory(external/glm)
target_link_libraries(${PROJECT_NAME} PUBLIC glm::glm)

# not on emscripten
if (NOT EMSCRIPTEN)
    # boxer
    add_subdirectory(external/boxer)
    target_link_libraries(${PROJECT_NAME} PUBLIC Boxer)

endif ()


# Add optional examples directory. Examples are built only when BUILD_LGE_EXAMPLES is ON.
option(BUILD_LGE_EXAMPLES "Build example programs" OFF)
if (BUILD_LGE_EXAMPLES)
    add_subdirectory(examples)
endif ()
