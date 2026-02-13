// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/app_config.hpp>
#include <lge/result.hpp>

#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float4.hpp>
#include <string>

namespace lge {

class renderer {
public:
	explicit renderer() = default;
	virtual ~renderer() = default;

	renderer(const renderer &) = delete;
	renderer(renderer &&) = delete;
	auto operator=(const renderer &) -> renderer & = delete;
	auto operator=(renderer &&) -> renderer & = delete;

	[[nodiscard]] virtual auto init(const app_config &config) -> result<> = 0;
	[[nodiscard]] virtual auto end() -> result<> = 0;

	[[nodiscard]] virtual auto begin_frame() -> result<> = 0;
	[[nodiscard]] virtual auto end_frame() const -> result<> = 0;

	[[nodiscard]] virtual auto should_close() const -> bool = 0;

	[[nodiscard]] virtual auto is_fullscreen() -> bool = 0;
	virtual auto set_fullscreen(bool fullscreen) -> void = 0;
	virtual auto toggle_fullscreen() -> void = 0;

	virtual auto render_label(const std::string &text,
							  const int &size,
							  const glm::vec4 &color,
							  const glm::vec2 &position,
							  float rotation) const -> void = 0;

	virtual auto render_quad(const glm::vec2 &p0,
							 const glm::vec2 &p1,
							 const glm::vec2 &p2,
							 const glm::vec2 &p3,
							 const glm::vec4 &color) const -> void = 0;

	virtual auto render_rect(const glm::vec2 &from,
							 const glm::vec2 &to,
							 const glm::vec4 &border_color,
							 const glm::vec4 &fill_color,
							 float border_thickness) const -> void = 0;

	virtual auto render_circle(const glm::vec2 &center,
							   float radius,
							   const glm::vec4 &border_color,
							   const glm::vec4 &fill_color,
							   float border_thickness) const -> void = 0;

	virtual auto get_label_size(const std::string &text, const int &size) -> glm::vec2 = 0;

	virtual auto show_cursor(bool show) -> void = 0;

	virtual auto get_delta_time() -> float = 0;

	auto set_debug_draw(const bool debug_draw) -> void {
		debug_draw_ = debug_draw;
	}

	[[nodiscard]] auto is_debug_draw() const -> bool {
		return debug_draw_;
	}

	auto toggle_debug_draw() -> void {
		debug_draw_ = !debug_draw_;
	}

private:
	bool debug_draw_ = false;
};

} // namespace lge
