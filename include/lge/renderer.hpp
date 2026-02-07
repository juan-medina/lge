// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/app_config.hpp>
#include <lge/components/aabb.hpp>
#include <lge/components/label.hpp>
#include <lge/result.hpp>

#include <raylib.h>

#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float4.hpp>
#include <string>

namespace lge {

class renderer {
public:
	explicit renderer() = default;
	~renderer() = default;

	renderer(const renderer &) = delete;
	renderer(renderer &&) = delete;
	auto operator=(const renderer &) -> renderer & = delete;
	auto operator=(renderer &&) -> renderer & = delete;

	[[nodiscard]] auto init(const app_config &config) -> result<>;
	[[nodiscard]] auto end() -> result<>;

	[[nodiscard]] auto begin_frame() -> result<>;
	[[nodiscard]] auto end_frame() const -> result<>;

	[[nodiscard]] auto should_close() const -> bool;

	[[nodiscard]] static auto is_fullscreen() -> bool;
	static auto set_fullscreen(bool fullscreen) -> void;
	static auto toggle_fullscreen() -> void;

	static auto setup_raylib_log() -> void;
	static auto get_delta_time() -> float;

	auto render_label(const label &label, const glm::vec2 &position) const -> void;

	auto render_aabb(global_aabb ga) const -> void;

	auto set_debug_draw(bool debug_draw) -> void {
		debug_draw_ = debug_draw;
	}

	[[nodiscard]] auto is_debug_draw() const -> bool {
		return debug_draw_;
	}

	auto toggle_debug_draw() -> void {
		debug_draw_ = !debug_draw_;
	}

	static auto get_label_size(const label &lbl) -> glm::vec2;

	static auto show_cursor(bool show) -> void;

private:
	static auto log_callback(int log_level, const char *text, va_list args) -> void; // NOLINT(*-include-cleaner)

	bool initialized_ = false;

	std::string title_;
	Color clear_color_ = BLACK;
	glm::vec2 screen_size_{};
	glm::vec2 design_resolution_{};
	glm::vec2 drawing_resolution_{};
	float scale_factor_{1.0F};
	RenderTexture2D render_texture_{};

	[[nodiscard]] auto screen_size_changed(glm::vec2 screen_size) -> result<>;

	static auto color_from_glm(const glm::vec4 &v) -> Color {
		return ColorFromNormalized({v.r, v.g, v.b, v.a});
	}

	[[nodiscard]] auto to_screen(const glm::vec2 &p) const -> glm::vec2 {
		return {p.x + (drawing_resolution_.x * 0.5F), p.y + (drawing_resolution_.y * 0.5F)};
	}

	bool debug_draw_ = false;
};

} // namespace lge
