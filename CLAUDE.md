# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

**lge** is a small C++20 2D game engine wrapping raylib for cross-platform development (Windows, Linux, macOS, WebAssembly/Emscripten). It follows YAGNI — only implement what's needed.

## Build Commands

```bash
# Windows (MSVC)
cmake -B cmake-build-msvc-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build cmake-build-msvc-debug

# GCC/Clang
cmake -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build cmake-build-debug

# WebAssembly (Emscripten)
emcmake cmake -B cmake-build-emscripten-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build cmake-build-emscripten-debug

# Build with examples
cmake -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug -DBUILD_LGE_EXAMPLES=ON
```

There is no formal test suite; testing is manual playtesting with debug builds.

## Tooling

```bash
# Format code (runs clang-format on src/)
./run_clang-format.sh

# Check includes (runs include-what-you-use)
./run_iwyu.sh
```

clang-tidy runs with strict rules and all warnings must be cleared before committing.

## Architecture

The engine is a static library (`lge`) with public headers in `include/lge/` and implementation in `src/lge/`.

### Core Pattern: ECS with Phase-Based Systems

- **App** (`lge::app`): Virtual base class for games. Manages an `entt::registry` (ECS world). Lifecycle: `init()` → `run()` → `end()`.
- **Systems** (`lge::system`): Registered with phases — `game_update`, `local_update`, `global_update`, `render`, `post_render`. Each implements `update(float dt) -> result<>`.
- **Components**: Data-only structs. Public ones in `include/lge/components/` (label, placement, hidden, order, hierarchy, shapes). Internal ones in `src/lge/internal/components/` (transform, bounds, metrics, render_order, effective_hidden).

### Backend Abstraction

Interfaces in `include/lge/interface/` (Renderer, Input, ResourceManager) with raylib implementations hidden in `src/lge/internal/raylib/`. Games code against the interfaces.

### Error Handling

No exceptions. Uses `lge::result<T>` (variant-based, like C++23 `expected`) and `lge::error` with stack traces. Errors propagate up and are logged at the top of the stack.

### Rendering

Fixed design resolution scaled to window. Wide screens get extended horizontal view (no black bars, no distortion). Input is mapped to game resolution. Games handle `layout` events for repositioning on resolution changes.

### Logging

Use the `lge::log` class: `log::debug("msg: {}", val)`, `log::info(...)`, `log::warn(...)`, `log::error(...)`, `log::trace(...)`. Log level is compile-time filtered via `LGE_ACTIVE_LOG_LEVEL` CMake option (DEBUG in Debug builds, ERROR in Release). Do not use logging for error propagation.

### Dependencies (vendored in `external/` as submodules)

- **raylib**: Graphics/windowing (private dependency)
- **entt**: Header-only ECS (public)
- **glm**: Math — vec2, vec4, etc. (public)
- **spdlog**: Logging, uses `std::format` via `SPDLOG_USE_STD_FORMAT` (public)
- **Boxer**: Message boxes, not available on Emscripten (public, conditional)

## Code Conventions

- **Naming**: `snake_case` for everything (classes, functions, variables, enums, namespaces). `UPPER_CASE` for macros. `Camel_Snake_Case` for template parameters. Private members have trailing underscore (`name_`).
- **Includes**: Project headers with quotes (`"lge/..."`), system/third-party with angle brackets (`<vector>`, `<entt/entt.hpp>`). All includes must be explicit. Use clang-format to sort.
- **Headers vs .cpp**: Public API, templates, and inline methods in headers. Implementation in .cpp files.
- **Section comments**: Use `// ====...` bars to group related members/methods.
- **`[[nodiscard]]`**: On getters and pure functions. **`noexcept`**: On hot-path functions.
- **`[[likely]]`/`[[unlikely]]`**: In performance-critical branches.
- **No**: exceptions, RTTI, raw pointers, manual memory management.
- **Comments**: Code should be self-explanatory. Only comment non-obvious logic. No "what changed" comments.

## Performance Rules

- **Hot paths** (render/update loops): Avoid allocations, pre-allocate containers, use `std::array`/stack buffers, `std::string_view`, mark functions `noexcept`.
- **Cold paths** (init, loading, error handling): STL containers and allocations are fine for clarity.

## Application Helper (cmake/LGEApplication.cmake)

Games use `lge_add_application(NAME ... SOURCES ... RESOURCES ...)` to create executables. It handles resource merging, platform-specific settings, and Emscripten shell/asset configuration.

## Testing

Testing in lge is pragmatic and targeted. There is no formal test suite; manual playtesting is standard. Engine internals that have no dependency on raylib or a render context are covered by tests. The goal is to catch regressions in subsystems where bugs are silent and hard to diagnose—not to achieve coverage metrics.

**What is tested:**

- Pure logic with no external dependencies (math, transforms, hierarchy, error propagation)
- Systems that can be exercised with a real `entt::registry` and no render context
- Anything that has previously caused a hard-to-diagnose bug

**What is not tested:**

- Anything requiring a raylib window or render context
- Gameplay feel, timing, or visual correctness
- Scene logic or application lifecycle

Tests use a real `entt::registry`—no mocking. If a system needs a window, it is not tested.

**Build with tests enabled:**

```bash
cmake -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug -DLGE_BUILD_TESTS=ON
cmake --build cmake-build-debug
```

**Run tests:**

```bash
ctest --test-dir cmake-build-debug/tests --output-on-failure
```

Tests run automatically on every push via GitHub Actions across Linux, macOS, and Windows.

## Test Addition Policy

Do not add new tests unless they strictly adhere to the project's testing philosophy:

- Only add tests for pure logic, engine internals without external dependencies, or subsystems where regressions are silent and hard to diagnose.
- Do not add tests for code requiring a render context, gameplay feel, timing, visual correctness, or scene/application lifecycle.
- All new tests must be justified by prior bugs or clear risk of silent failure.
- If unsure, do not add the test and ask for clarification.
