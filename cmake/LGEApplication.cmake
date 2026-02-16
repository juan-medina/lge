# SPDX-FileCopyrightText: 2026 Juan Medina
# SPDX-License-Identifier: MIT

# LGEApplication.cmake
# Reusable CMake configuration for applications using the lge engine.
#
# Usage:
#   include(<path-to-lge>/cmake/LGEApplication.cmake)
#   lge_add_application(<target_name>
#       SOURCES <source_files...>
#       [EMSCRIPTEN_SHELL <path-to-shell-template>]
#       [EMSCRIPTEN_ASSETS <asset_files...>]
#       [GAME_RESOURCES <path-to-game-resources>]
#   )
#
# Parameters:
#   <target_name>         - Name of the application target (required)
#   SOURCES               - List of source files for the application (required)
#   EMSCRIPTEN_SHELL      - Path to custom HTML shell template for Emscripten builds (optional)
#   EMSCRIPTEN_ASSETS     - List of asset files to copy for Emscripten builds (optional)
#   GAME_RESOURCES        - Path to the game's resources folder to merge with engine resources (optional)
#
# This function will:
#   - Always copy engine resources from /resources to resources/lge in the build output
#   - Optionally copy game resources to resources/game if GAME_RESOURCES is provided
#   - For Emscripten, set up the merged resources folder as a preload file
#   - Resources are refreshed on every build
#
# Example:
#   lge_add_application(my_game
#       SOURCES ${GAME_SOURCES}
#       EMSCRIPTEN_SHELL ${CMAKE_CURRENT_SOURCE_DIR}/web/template.html
#       EMSCRIPTEN_ASSETS ${CMAKE_CURRENT_SOURCE_DIR}/web/favicon.ico
#       GAME_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/resources
#   )

function(lge_add_application TARGET_NAME)
    set(options "")
    set(oneValueArgs EMSCRIPTEN_SHELL GAME_RESOURCES)
    set(multiValueArgs SOURCES EMSCRIPTEN_ASSETS)
    cmake_parse_arguments(LGE_APP "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT LGE_APP_SOURCES)
        message(FATAL_ERROR "lge_add_application: SOURCES must be specified")
    endif ()

    # Set log levels
    target_compile_definitions(lge PUBLIC LGE_ACTIVE_LOG_LEVEL=${LGE_ACTIVE_LOG_LEVEL})
    target_compile_definitions(lge PUBLIC SPDLOG_ACTIVE_LEVEL=${LGE_ACTIVE_LOG_LEVEL})

    # Create executable
    add_executable(${TARGET_NAME})
    target_sources(${TARGET_NAME} PRIVATE ${LGE_APP_SOURCES})
    target_link_libraries(${TARGET_NAME} PRIVATE lge)

    # =============================================================================
    # Resource Merging
    # =============================================================================
    set(MERGED_RESOURCES_DIR "${CMAKE_CURRENT_BINARY_DIR}/resources")
    set(ENGINE_RESOURCES_SRC "${CMAKE_SOURCE_DIR}/resources")
    set(ENGINE_RESOURCES_DST "${MERGED_RESOURCES_DIR}/lge")

    # Create resource copy target (unique per application)
    set(COPY_RESOURCES_TARGET "copy_resources_${TARGET_NAME}")

    if (LGE_APP_GAME_RESOURCES)
        set(GAME_RESOURCES_SRC "${LGE_APP_GAME_RESOURCES}")
        set(GAME_RESOURCES_DST "${MERGED_RESOURCES_DIR}/game")

        add_custom_target(${COPY_RESOURCES_TARGET} ALL
                COMMAND ${CMAKE_COMMAND} -E remove_directory "${ENGINE_RESOURCES_DST}"
                COMMAND ${CMAKE_COMMAND} -E copy_directory "${ENGINE_RESOURCES_SRC}" "${ENGINE_RESOURCES_DST}"
                COMMAND ${CMAKE_COMMAND} -E remove_directory "${GAME_RESOURCES_DST}"
                COMMAND ${CMAKE_COMMAND} -E copy_directory "${GAME_RESOURCES_SRC}" "${GAME_RESOURCES_DST}"
                COMMENT "Refreshing engine and game resources for ${TARGET_NAME}"
                VERBATIM
        )
    else ()
        add_custom_target(${COPY_RESOURCES_TARGET} ALL
                COMMAND ${CMAKE_COMMAND} -E remove_directory "${ENGINE_RESOURCES_DST}"
                COMMAND ${CMAKE_COMMAND} -E copy_directory "${ENGINE_RESOURCES_SRC}" "${ENGINE_RESOURCES_DST}"
                COMMENT "Refreshing engine resources for ${TARGET_NAME}"
                VERBATIM
        )
    endif ()

    add_dependencies(${TARGET_NAME} ${COPY_RESOURCES_TARGET})

    # =============================================================================
    # Platform-specific Configuration
    # =============================================================================

    # Windows-specific settings
    if (WIN32)
        target_link_options(${TARGET_NAME} PRIVATE
                $<$<CONFIG:Debug>:/SUBSYSTEM:CONSOLE>
                $<$<NOT:$<CONFIG:Debug>>:/SUBSYSTEM:WINDOWS>
        )
        target_compile_definitions(${TARGET_NAME} PRIVATE
                WIN32_LEAN_AND_MEAN
                NOMINMAX
                NOUSER
                NOGDI
        )
    endif ()

    # Emscripten-specific settings
    if (EMSCRIPTEN)
        set_target_properties(${TARGET_NAME} PROPERTIES
                SUFFIX ".html"
                OUTPUT_NAME "index"
        )

        set(EMSCRIPTEN_LINK_OPTIONS
                "SHELL:-sUSE_GLFW=3"
                "SHELL:-sALLOW_MEMORY_GROWTH=1"
                "SHELL:--bind"
                "SHELL:-sEXPORTED_RUNTIME_METHODS=['HEAPF32','HEAP32','HEAPU8','requestFullscreen']"
                "SHELL:--preload-file=${MERGED_RESOURCES_DIR}@resources"
        )

        if (LGE_APP_EMSCRIPTEN_SHELL)
            list(APPEND EMSCRIPTEN_LINK_OPTIONS "SHELL:--shell-file=${LGE_APP_EMSCRIPTEN_SHELL}")
        endif ()

        target_link_options(${TARGET_NAME} PRIVATE ${EMSCRIPTEN_LINK_OPTIONS})

        # Copy Emscripten assets
        if (LGE_APP_EMSCRIPTEN_ASSETS)
            foreach (ASSET ${LGE_APP_EMSCRIPTEN_ASSETS})
                get_filename_component(ASSET_NAME "${ASSET}" NAME)
                configure_file("${ASSET}" "${CMAKE_CURRENT_BINARY_DIR}/${ASSET_NAME}" COPYONLY)
            endforeach ()
        endif ()
    endif ()
endfunction()