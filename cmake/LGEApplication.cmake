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
#   )

function(lge_add_application TARGET_NAME)
    set(options "")
    set(oneValueArgs EMSCRIPTEN_SHELL)
    set(multiValueArgs SOURCES EMSCRIPTEN_ASSETS)
    cmake_parse_arguments(LGE_APP "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT LGE_APP_SOURCES)
        message(FATAL_ERROR "lge_add_application: SOURCES must be specified")
    endif ()

    add_executable(${TARGET_NAME} ${LGE_APP_SOURCES})

    target_link_libraries(${TARGET_NAME} PRIVATE lge)

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

    if (EMSCRIPTEN)
        set(CMAKE_EXECUTABLE_SUFFIX ".html" PARENT_SCOPE)

        set(EMSCRIPTEN_LINK_OPTIONS
                "-sUSE_GLFW=3"
                "-sALLOW_MEMORY_GROWTH=1"
                "--bind"
                "-sEXPORTED_RUNTIME_METHODS=['HEAPF32','HEAP32','HEAPU8','requestFullscreen']"
        )

        if (LGE_APP_EMSCRIPTEN_SHELL)
            list(APPEND EMSCRIPTEN_LINK_OPTIONS "--shell-file=${LGE_APP_EMSCRIPTEN_SHELL}")
        endif ()

        target_link_options(${TARGET_NAME} PRIVATE ${EMSCRIPTEN_LINK_OPTIONS})

        set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME "index")

        foreach (ASSET ${LGE_APP_EMSCRIPTEN_ASSETS})
            get_filename_component(ASSET_NAME ${ASSET} NAME)
            configure_file(${ASSET} ${ASSET_NAME} COPYONLY)
        endforeach ()
    endif ()
endfunction()

