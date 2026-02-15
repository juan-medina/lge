- Header/Implementation Split: Public API, templates, and inline methods are in the header; implementation logic is in the .cpp file.
- Include Order: Project headers first, then system/third-party headers, all sorted and grouped. Use angle brackets for dependencies, quotes for project headers.
- Explicit Includes: All used types (e.g., `std::string`, `std::vector`) are explicitly included, even if transitively included elsewhere.
- Section Comments: Use clear section comments (e.g., `// =============================================================================`) to group related members and methods.
- Access Control: Public, protected, and private members are clearly separated.
- Method Grouping: Related methods (event, audio, rendering, etc.) are grouped together for clarity.
- Const-Correctness: Use `const` and `[[nodiscard]]` where appropriate for getters and pure functions.
- Template Usage: Templates are used for generic event and scene registration APIs, defined inline in the header.
- Error Handling: All functions returning a result use `lge::result` and propagate errors up, never exceptions.
- Logging: Use LGE_DEBUG,LGE_INFO  macros for debug/info logging etc, never for error propagation (errors are handled via result/error).
- Naming Conventions: Use snake_case for variables and methods, PascalCase for types/classes, and trailing underscores for private members (e.g., `name_`).

- Method Decomposition: Break down large or complex methods into smaller, focused helpers that each express a single idea.
- Static Methods: Any function that does not use instance state should be made static, except event handlers or callbacks that must remain instance methods.

- Comment Policy:
	- Do not add comments about what the code is doing or why something was changed (e.g., "now does X instead of Y").
	- Code should be self-explanatory through clear naming and structure; only add comments for non-obvious or complex logic.
	- Rationale for changes belongs in commit messages or code review, not in the code itself.
	- Preserve existing comments when refactoring, unless they are no longer accurate or relevant. Add new comments only for non-obvious logic; avoid comments that simply restate code.

- Copilot Behavior:
	- Make minimal, targeted edits; do not rewrite or reformat entire files unless explicitly requested.
	- Follow the existing structure, naming, and idioms of the file being edited.
	- When intent is unclear, prefer asking for clarification over guessing.
	- Do not introduce new dependencies, libraries, or abstractions unless explicitly requested.
	- Keep diffs small and avoid unrelated changes.
	- Generate code that compiles under C++20 and follows all project conventions.

- Refactoring Policy:
	- Do not change the functional behavior of code during refactoring unless explicitly requested or required.
	- Do not rename identifiers, reorder methods, or restructure files unless the request is specifically about organization.
	- Include Preservation: When refactoring, do not change the list of #include directives unless required for correctness or explicit includes.
	- Avoid "optimizing" or "simplifying" logic unless explicitly asked.

## Development Philosophy
- The codebase follows the YAGNI (You Aren't Gonna Need It) principle: only implement features when they are truly needed.
- Avoid speculative abstractions or overengineering; keep systems minimal until requirements demand more.
- Example: No ECS system is present—just a vector of components—because a full ECS is not currently required.

## Modern C++ Practices
- Do not use old C++ or C patterns (e.g., no raw pointers, no custom vector/collection types, no manual memory management).
- Always prefer standard C++20 features and containers (e.g., `std::vector`, `std::unique_ptr`).
- No exceptions; no RTTI (`dynamic_cast`, `typeid`).
- Prefer `constexpr`, `std::span`, `std::optional`, and const-correctness.
- Use `[[nodiscard]]` where appropriate to prevent ignoring important return values.
- Mark all functions `noexcept` where applicable, especially in hot paths and performance-critical code.
- Use branch prediction hints `[[likely]]` and `[[unlikely]]` in performance-critical paths where branch outcomes are predictable.
- Prefer `std::array` over `std::vector` when size is known at compile time.
- Use `std::string_view` instead of `const std::string&` for read-only string parameters.
- Leverage `if constexpr` for compile-time conditionals in templates.

## Game Development Performance Guidelines
- **Hot Path Optimization**: Code in the render loop, update loop, or called hundreds of times per frame must be optimized for performance.
- **Container Usage in Hot Paths**:
	- Avoid `std::vector`, `std::map`, `std::unordered_map` allocations in per-frame code.
	- Pre-allocate containers outside loops and reuse them via `clear()` rather than reconstructing.
	- Use stack-based `std::array` or fixed-size buffers when possible.
	- For rendering: z-order sorting with `std::vector` is acceptable as it happens once per frame; excessive per-entity container creation is not.
- **Memory Allocations**: Minimize allocations in performance-critical code. Prefer stack allocation, object pools, or pre-allocated buffers.
- **Branch Prediction**: Use `[[likely]]` and `[[unlikely]]` in hot paths where branch outcomes are predictable (e.g., error checks that rarely fail).
- **Function Calls**: Mark hot path functions `noexcept` to eliminate exception handling overhead and enable compiler optimizations.
- **Data Layout**: Prefer data-oriented design where appropriate. Keep frequently accessed data contiguous in memory.
- **String Operations**: Avoid string concatenation, formatting, or allocations in per-frame code. Use `std::string_view` for read-only operations.
- **Non-Critical Code**: Initialization, loading, one-time setup, and error handling paths can freely use STL containers and allocations for clarity and maintainability.
- **Profile Before Optimizing**: Follow these guidelines in known hot paths, but profile before micro-optimizing cold paths.

## Project Overview
- This project is the lge engine, which wraps raylib and other libraries for cross-platform 2D game development.
- Main logic and assets are in `src/`.

## Architecture & Key Components
- `src/`: Main source code.
- `include/`: Public headers for the engine and game.
- `resources/`: Game assets (images, audio, etc.) loaded at runtime.
- `external/`: Vendored third-party libraries, including the lge engine.
- `CMakeLists.txt`: Main build configuration. Supports multiple platforms (Windows, Linux, macOS, WebAssembly).
- `examples/`: Example projects demonstrating engine usage.

## Build & Run
- Use CMake for all builds. Out-of-source builds are recommended (see `cmake-build-*` folders).
- Example build commands:
	- Windows (MSVC):
	  cmake -B cmake-build-msvc-debug -DCMAKE_BUILD_TYPE=Debug
	  cmake --build cmake-build-msvc-debug
	- WebAssembly (Emscripten):
	  emcmake cmake -B cmake-build-wsl2-emscripten-debug -DCMAKE_BUILD_TYPE=Debug
	  cmake --build cmake-build-wsl2-emscripten-debug

## Testing & Debugging
- No formal test suite; manual playtesting is standard.
- Debug builds enable extra logging and assertions.
- Use the `resources/` directory for hot-reloading assets during development.

## Tooling
- The codebase uses clang-tidy with strict warning settings. All warnings must be cleared before committing code.
- The active clang-tidy rules are defined in the project configuration.
- Run clang-tidy as part of the workflow to ensure compliance with project standards.
- Use clang-format to format code and sort includes automatically.
- All includes must be necessary—remove unused includes and add explicit includes for all used types.
- All third-party and standard library includes must use angle brackets: `#include <...>`.
- Only game/project headers use quotes: `#include "..."` with relative paths.

## Error and Result Handling
- All error and result handling uses `lge::result` and `lge::error` (similar to C++23 `expected`, but project-specific).
- Do not use exceptions; avoid code that throws or relies on exception handling.
- When propagating errors, construct new errors from previous ones using the error constructor, but do not extract or log the previous error's message directly.
- Logging is done via spdlog. Use debug logs for development information, and only minimal info-level logs for user-facing events.
- Do not log errors directly; errors are propagated via result/error and logged at the top engine/stack with stack trace included.
- Error handling paths are not performance-critical; clarity and proper error propagation take priority over micro-optimizations.

## Rendering & Layout Philosophy
- The engine renders the entire game at a small, fixed resolution, then scales to fit the window/screen size.
- No aspect ratio distortion or black bars: the view is extended horizontally to show more content on wider screens.
- Input (e.g., mouse) is mapped/scaled to the game resolution.
- Game scenes receive a `layout` event on resolution changes, allowing them to reposition/move graphics and UI elements as needed.

## Examples
- See `/include/lge/app/app.hpp` & `src/lge/app/app.cpp` for an example of class & formatting conventions.

## Additional Notes
- All contributions must follow the style and structure of existing code.
- When in doubt, ask for clarification rather than making assumptions about intent or style.