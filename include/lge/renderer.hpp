// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/result.hpp>

#include <raylib.h>

#include <glm/ext/vector_float2.hpp>
#include <glm/vec2.hpp>

namespace lge {

class renderer {
public:
	explicit renderer() = default;
	~renderer() = default;

	renderer(const renderer &) = delete;
	renderer(renderer &&) = delete;
	auto operator=(const renderer &) -> renderer & = delete;
	auto operator=(renderer &&) -> renderer & = delete;

	[[nodiscard]] auto init() -> result<>;
	[[nodiscard]] auto end() -> result<>;

	[[nodiscard]] auto begin_frame() -> result<>;
	[[nodiscard]] auto end_frame() const -> result<>;

	[[nodiscard]] auto should_close() const -> bool;

	[[nodiscard]] static auto is_fullscreen() -> bool;
	static auto set_fullscreen(bool fullscreen) -> void;
	static auto toggle_fullscreen() -> void;

	static auto setup_raylib_log() -> void;
	static auto get_delta_time() -> float;

private:
	static auto log_callback(int log_level, const char *text, va_list args) -> void; // NOLINT(*-include-cleaner)

	bool initialized_ = false;

	Color clear_color_{BLACK};
	glm::vec2 screen_size_{};
	glm::vec2 design_resolution_{640, 360};
	glm::vec2 drawing_resolution_{};
	float scale_factor_{1.0F};
	RenderTexture2D render_texture_{};

	[[nodiscard]] auto screen_size_changed(glm::vec2 screen_size) -> result<>;
};

} // namespace lge
