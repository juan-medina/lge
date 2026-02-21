# l[g]e : little game engine

[![License: MIT](https://img.shields.io/badge/License-MIT-478CBF.svg?style=for-the-badge)](https://opensource.org/licenses/MIT)
[![Conduct](https://img.shields.io/badge/Conduct-Covenat%202.0-478CBF.svg?style=for-the-badge)](https://www.contributor-covenant.org/version/2/0/code_of_conduct/)
[![Made with C++20](https://img.shields.io/badge/C%2B%2B-20-478CBF?style=for-the-badge&logo=c%2B%2B&logoColor=white)](https://isocpp.org/)

<p align ="center">
  <img src="media/logo/logo-sm.png" width="110" alt="lge logo">
</p>

An little 2D game engine built in C++20 using raylib.

---

## Using lge in Your Project

### Adding lge as a Submodule

Add lge to your project as a Git submodule:

```bash
git submodule add https://github.com/juan-medina/lge.git external/lge
git submodule update --init --recursive
```

### CMake Integration

In your root `CMakeLists.txt`, add lge as a subdirectory and use the provided helper:

```cmake
cmake_minimum_required(VERSION 3.28)

project(my_game
	VERSION 0.1.0.0
	LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add lge engine
add_subdirectory(external/lge)

# Include the lge application helper
include(external/lge/cmake/LGEApplication.cmake)

# Define your application
file(GLOB_RECURSE GAME_SOURCES "src/*.cpp")

# Optionally specify your own resources folder
set(GAME_RESOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/resources")

lge_add_application(my_game
	SOURCES ${GAME_SOURCES}
	EMSCRIPTEN_SHELL ${CMAKE_CURRENT_SOURCE_DIR}/web/template.html
	EMSCRIPTEN_ASSETS ${CMAKE_CURRENT_SOURCE_DIR}/web/favicon.ico
	GAME_RESOURCES ${GAME_RESOURCES_DIR}
)
```

### lge_add_application Options

| Option              | Required | Description                                                         |
|---------------------|----------|---------------------------------------------------------------------|
| `SOURCES`           | Yes      | List of source files for your application                           |
| `EMSCRIPTEN_SHELL`  | No       | Path to custom HTML shell template for WebAssembly builds           |
| `EMSCRIPTEN_ASSETS` | No       | List of asset files to copy for WebAssembly builds                  |
| `GAME_RESOURCES`    | No       | Path to your game's resources folder (merged with engine resources) |

The function automatically configures:

- Linking to the lge library
- Windows subsystem settings (console for Debug, Windows for Release)
- Emscripten/WebAssembly build options (GLFW, memory growth, exports)
- Merges engine resources and your game resources into the build output directory:
	- `resources/lge` (engine)
	- `resources/game` (your game, if provided)
- For Emscripten, preloads the merged resources folder for use in the browser

### Minimal Example

**src/my_game.hpp**

```cpp
#pragma once

#include <lge/app/app.hpp>
#include <lge/core/result.hpp>

class my_game: public lge::app {
public:
    [[nodiscard]] auto configure() -> lge::app_config override;
    [[nodiscard]] auto init() -> lge::result<> override;
};
```

**src/my_game.cpp**

```cpp
#include "my_game.hpp"

#include <lge/app/app_config.hpp>
#include <lge/components/label.hpp>
#include <lge/components/placement.hpp>
#include <lge/app/main.hpp>
#include <lge/core/result.hpp>

LGE_MAIN(my_game);

auto my_game::configure() -> lge::app_config {
    return {
        .design_resolution = {640, 360}, // world expands horizontally, but height is fixed
        .clear_color = {0, 0, 0, 1},
        .window_title = "My Game"
    };
}

auto my_game::init() -> lge::result<> {
    if (const auto err = app::init().unwrap(); err) {
        return lge::error("error initializing app", *err);
    }

    auto& world = get_world();

    const auto label_ent = world.create();
    world.emplace<lge::label>(label_ent, "Hello, lge!");
    world.emplace<lge::placement>(label_ent, 0.0F, 0.0F); // center of the screen

    return true;
}
```

---

**Credits**

- Developed by [Juan Medina](https://juan-medina.com).

**Third-party libraries:**

- [raylib](https://www.raylib.com/) (licensed under zlib/libpng License)
- [entt](https://github.com/skypjack/entt) (licensed under MIT License)
- [glm](https://github.com/g-truc/glm) (licensed under The Happy Bunny License (Modified MIT License))
- [spdlog](https://github.com/gabime/spdlog) (licensed under MIT License)
- [Boxer](https://github.com/aaronmjacobs/Boxer) (licensed under MIT License)
- [jsoncons](https://github.com/danielaparker/jsoncons) (licensed under Boost Software License)

**Graphics**

*Engine assets:*

- [Peaberry Pixel Font by emhuo](https://emhuo.itch.io/peaberry-pixel-font) (licensed under Open Font License Version
  1.1).

*Example assets:*

- [Pixelart Hiker by Chroma Dave](https://chroma-dave.itch.io/pixelart-hiker) (free to use in any project per the
  author).
- [Demon Woods Parallax Background by Aethrall](https://aethrall.itch.io/demon-woods-parallax-background) (set as free
  by the author).

**License**

- This project is licensed under the MIT License — see `LICENSE` for details.

**Contributing**

- Feel free to open issues or pull requests. For code changes, follow the existing style and keep changes focused.

**Contact**

- Author: Juan Medina (copyright 2026)
