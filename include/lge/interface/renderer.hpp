// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/app/app_config.hpp>
#include <lge/core/colors.hpp>
#include <lge/core/result.hpp>
#include <lge/interface/resource_manager.hpp>

#include <entt/core/fwd.hpp>
#include <glm/ext/vector_float2.hpp>
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

	virtual auto render_label(font_handle font,
							  const std::string &text,
							  const int &size,
							  const color &color,
							  const glm::vec2 &pivot_position,
							  const glm::vec2 &rotated_offset,
							  float rotation) const -> void = 0;

	virtual auto render_quad(const glm::vec2 &p0,
							 const glm::vec2 &p1,
							 const glm::vec2 &p2,
							 const glm::vec2 &p3,
							 const color &color) const -> void = 0;

	virtual auto render_rect(const glm::vec2 &center,
							 const glm::vec2 &size,
							 float rotation,
							 const color &border_color,
							 const color &fill_color,
							 float border_thickness) const -> void = 0;

	virtual auto render_circle(const glm::vec2 &center,
							   float radius,
							   const color &border_color,
							   const color &fill_color,
							   float border_thickness) const -> void = 0;

	virtual auto render_sprite(sprite_sheet_handle sheet,
							   entt::id_type frame,
							   const glm::vec2 &pivot_position,
							   const glm::vec2 &size,
							   const glm::vec2 &pivot,
							   float rotation,
							   bool flip_horizontal,
							   bool flip_vertical) const -> void = 0;

	virtual auto get_label_size(font_handle font, const std::string &text, const int &size) -> glm::vec2 = 0;

	virtual auto get_texture_size(texture_handle texture) -> glm::vec2 = 0;

	virtual auto get_sprite_frame_size(sprite_sheet_handle sheet, entt::id_type frame) -> glm::vec2 = 0;

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