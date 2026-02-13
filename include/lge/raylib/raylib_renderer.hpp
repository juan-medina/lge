// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/app_config.hpp>
#include <lge/renderer.hpp>
#include <lge/result.hpp>

#include <raylib.h>

#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float4.hpp>
#include <string>

namespace lge {

class raylib_renderer: public renderer {
public:
	explicit raylib_renderer() = default;
	~raylib_renderer() override = default;

	raylib_renderer(const raylib_renderer &) = delete;
	raylib_renderer(raylib_renderer &&) = delete;
	auto operator=(const raylib_renderer &) -> raylib_renderer & = delete;
	auto operator=(raylib_renderer &&) -> raylib_renderer & = delete;

	[[nodiscard]] auto init(const app_config &config) -> result<> override;
	[[nodiscard]] auto end() -> result<> override;

	[[nodiscard]] auto begin_frame() -> result<> override;
	[[nodiscard]] auto end_frame() const -> result<> override;

	[[nodiscard]] auto should_close() const -> bool override;

	[[nodiscard]] auto is_fullscreen() -> bool override;
	auto set_fullscreen(bool fullscreen) -> void override;
	auto toggle_fullscreen() -> void override;

	auto get_label_size(const std::string &text, const int &size) -> glm::vec2 override;

	auto show_cursor(bool show) -> void override;

	auto get_delta_time() -> float override;

	auto render_label(const std::string &text,
					  const int &size,
					  const glm::vec4 &color,
					  const glm::vec2 &position,
					  float rotation) const -> void override;

	auto render_quad(const glm::vec2 &p0,
					 const glm::vec2 &p1,
					 const glm::vec2 &p2,
					 const glm::vec2 &p3,
					 const glm::vec4 &color) const -> void override;

	auto render_rect(const glm::vec2 &from,
							 const glm::vec2 &to,
							 const glm::vec4 &border_color,
							 const glm::vec4 &fill_color,
							 float border_thickness) const -> void override;

	auto render_circle(const glm::vec2 &center,
							   float radius,
							   const glm::vec4 &border_color,
							   const glm::vec4 &fill_color,
							   float border_thickness) const -> void override;

private:
	static auto setup_raylib_log() -> void;
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
};

} // namespace lge
