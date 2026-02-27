# SPDX-FileCopyrightText: 2026 Juan Medina
# SPDX-License-Identifier: MIT

project(example_06_collisions
        VERSION 0.1.0.0
        DESCRIPTION "example 06 - collisions"
        LANGUAGES CXX
)

file(GLOB_RECURSE APP_SOURCE_FILES "src/*.cpp")
file(GLOB_RECURSE COMMON_SOURCE_FILES "../src/*.cpp")

include(${CMAKE_SOURCE_DIR}/cmake/LGEApplication.cmake)

lge_add_application(${PROJECT_NAME}
        SOURCES ${APP_SOURCE_FILES} ${COMMON_SOURCE_FILES}
        EMSCRIPTEN_SHELL ${CMAKE_CURRENT_SOURCE_DIR}/../web/template.html
        EMSCRIPTEN_ASSETS
        ${CMAKE_CURRENT_SOURCE_DIR}/../web/favicon.ico
        ${CMAKE_CURRENT_SOURCE_DIR}/../web/favicon.svg
        ${CMAKE_CURRENT_SOURCE_DIR}/../web/favicon-96x96.png
        ${CMAKE_CURRENT_SOURCE_DIR}/../web/apple-touch-icon.png
        ${CMAKE_CURRENT_SOURCE_DIR}/../web/web-app-manifest-192x192.png
        ${CMAKE_CURRENT_SOURCE_DIR}/../web/web-app-manifest-512x512.png
        ${CMAKE_CURRENT_SOURCE_DIR}/../web/site.webmanifest
        APP_ICON_WIN ${CMAKE_CURRENT_SOURCE_DIR}/../web/favicon.ico
        APP_ICON_MAC ${CMAKE_CURRENT_SOURCE_DIR}/../web/web-app-manifest-512x512.png
        GAME_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../resources
)

