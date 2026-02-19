// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "raylib_renderer.hpp"

#include <lge/app/app_config.hpp>
#include <lge/core/log.hpp>
#include <lge/core/result.hpp>
#include <lge/interface/resource_manager.hpp>

#include "raylib_resource_manager.hpp"

#include <raylib.h>

#include <cstdarg>
#include <cstdio>
#include <glm/ext/vector_float4.hpp>
#include <glm/trigonometric.hpp>
#include <spdlog/common.h>
#include <spdlog/spdlog.h>
#include <string_view>
#include <vector>

namespace lge {

auto raylib_renderer::init(const app_config &config) -> result<> {
	clear_color_ = color_from_glm(config.clear_color);
	design_resolution_ = config.design_resolution;
	title_ = config.window_title;

	setup_raylib_log();

#if defined(_WIN32) || defined(__EMSCRIPTEN__)
	InitWindow(1920, 1080, title_.c_str());
#else
	InitWindow(0, 0, title_.c_str());
#endif
	SetWindowState(FLAG_WINDOW_RESIZABLE);

#ifndef __EMSCRIPTEN__
	if(!IsWindowReady()) [[unlikely]] {
		return error("failed to initialize window");
	}
#endif

	SetExitKey(KEY_NULL);
	SetTargetFPS(60);

	initialized_ = true;

	if(const auto err = resource_manager_.load_font(default_font_path).unwrap(default_font_); err) [[unlikely]] {
		return error("failed to load default font", *err);
	}

	log::info("raylib_renderer initialized");
	return true;
}

auto raylib_renderer::end() -> result<> {
	if(!initialized_) [[unlikely]] {
		return true;
	}

	if(default_font_.is_valid()) [[likely]] {
		if(const auto err = resource_manager_.unload_font(default_font_).unwrap(); err) [[unlikely]] {
			return error("failed to unload default font", *err);
		}
	}

	CloseWindow();
	initialized_ = false;

	if(render_texture_.id != 0) [[unlikely]] {
		UnloadRenderTexture(render_texture_);
	}

	log::info("raylib_renderer ended");
	return true;
}

auto raylib_renderer::begin_frame() -> result<> {
	if(!initialized_) [[unlikely]] {
		return error("raylib_renderer not initialized");
	}

	if(auto const screen_size = glm::vec2{static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())};
	   screen_size_.x != screen_size.x || screen_size_.y != screen_size.y) {
		if(auto const err = screen_size_changed(screen_size).unwrap(); err) [[unlikely]] {
			return error("failed to handle screen size change: {}", *err);
		}
	}

	BeginTextureMode(render_texture_);
	ClearBackground(clear_color_);

	return true;
}

auto raylib_renderer::end_frame() const -> result<> {
	if(!initialized_) [[unlikely]] {
		return error("raylib_renderer not initialized");
	}

	EndTextureMode();

	BeginDrawing();
	ClearBackground(clear_color_);

	DrawTexturePro(render_texture_.texture,
				   {.x = 0.0F,
					.y = 0.0F,
					.width = static_cast<float>(render_texture_.texture.width),
					.height = -static_cast<float>(render_texture_.texture.height)},
				   {.x = 0.0F, .y = 0.0F, .width = screen_size_.x, .height = screen_size_.y},
				   {.x = 0.0F, .y = 0.0F},
				   0.0F,
				   WHITE);

	EndDrawing();

	return true;
}

auto raylib_renderer::should_close() const -> bool {
	if(!initialized_) [[unlikely]] {
		return false;
	}

	return WindowShouldClose();
}

auto raylib_renderer::is_fullscreen() -> bool {
	bool full_screen = false;
#ifdef _WIN32
	full_screen = IsWindowState(FLAG_BORDERLESS_WINDOWED_MODE);
#else
	full_screen = IsWindowFullscreen();
#endif
	return full_screen;
}

auto raylib_renderer::set_fullscreen(const bool fullscreen) -> void {
	if(const auto current_state = is_fullscreen(); current_state != fullscreen) {
		toggle_fullscreen();
	}
}

auto raylib_renderer::toggle_fullscreen() -> void {
#ifdef WIN32
	ToggleBorderlessWindowed();
#else
	ToggleFullscreen();
#endif
}

auto raylib_renderer::setup_raylib_log() -> void {
	switch(LGE_ACTIVE_LOG_LEVEL) { // NOLINT(*-include-cleaner)
	case LGE_LOG_LEVEL_TRACE:
		SetTraceLogLevel(LOG_TRACE);
		break;
	case LGE_LOG_LEVEL_DEBUG:
		SetTraceLogLevel(LOG_DEBUG);
		break;
	case LGE_LOG_LEVEL_INFO:
		SetTraceLogLevel(LOG_INFO);
		break;
	case LGE_LOG_LEVEL_WARN:
		SetTraceLogLevel(LOG_WARNING);
		break;
	case LGE_LOG_LEVEL_ERROR:
		SetTraceLogLevel(LOG_ERROR);
		break;
	default:
		SetTraceLogLevel(LOG_NONE);
		break;
	}
	SetTraceLogCallback(log_callback);
}

auto raylib_renderer::get_delta_time() -> float {
	return GetFrameTime();
}

auto raylib_renderer::get_label_size(const font_handle font, const std::string &text, const int &size) -> glm::vec2 {
	auto rl_font = GetFontDefault();

	if(const auto font_to_use = font.is_valid() ? font : default_font_; font_to_use.is_valid()) {
		if(const auto err = resource_manager_.get_raylib_font(font_to_use).unwrap(rl_font); err) [[unlikely]] {
			log::error("failed to get font with id {}, rendering label with default font instead", font_to_use);
		}
	}

	const auto spacing = static_cast<float>(size) / static_cast<float>(rl_font.baseSize);

	const auto [width, height] = MeasureTextEx(rl_font, text.c_str(), static_cast<float>(size), spacing);
	return {width, height};
}

auto raylib_renderer::show_cursor(const bool show) -> void {
	if(show) {
		ShowCursor();
	} else {
		HideCursor();
	}
}

auto raylib_renderer::log_callback(const int log_level, const char *text, va_list args) -> void {
	constexpr std::size_t initial_size = 1024;
	thread_local std::vector<char> buffer(initial_size);

	va_list args_copy{};	  // NOLINT(cppcoreguidelines-pro-type-vararg)
	va_copy(args_copy, args); // NOLINT(*-pro-bounds-array-to-pointer-decay)
	int const needed =
		std::vsnprintf(buffer.data(), buffer.size(), text, args_copy); // NOLINT(*-pro-bounds-array-to-pointer-decay)
	va_end(args_copy);												   // NOLINT(*-pro-bounds-array-to-pointer-decay)

	if(needed < 0) [[unlikely]] {
		log::info("[raylib] log formatting error in log callback");
		return;
	}

	if(static_cast<std::size_t>(needed) >= buffer.size()) [[unlikely]] {
		buffer.resize(static_cast<std::size_t>(needed) + 1);
		std::vsnprintf(buffer.data(), buffer.size(), text, args);
	}

	spdlog::level::level_enum level = spdlog::level::info;
	switch(log_level) {
	case LOG_TRACE:
		level = spdlog::level::trace;
		break;
	case LOG_DEBUG:
		level = spdlog::level::debug;
		break;
	case LOG_INFO:
		level = spdlog::level::info;
		break;
	case LOG_WARNING:
		level = spdlog::level::warn;
		break;
	case LOG_ERROR:
		level = spdlog::level::err;
		break;
	case LOG_FATAL:
		level = spdlog::level::critical;
		break;
	default:
		break;
	}

	spdlog::log(level, "[raylib] " + std::string(buffer.data())); // raylib does not output a line number
}

auto raylib_renderer::screen_size_changed(const glm::vec2 screen_size) -> result<> {
	screen_size_ = screen_size;

	scale_factor_ = screen_size_.y / design_resolution_.y;
	drawing_resolution_.y = design_resolution_.y;
	drawing_resolution_.x = static_cast<float>(static_cast<int>(screen_size_.x / scale_factor_));

	log::debug("display resized, design resolution ({},{}) real resolution ({}x{}), drawing resolution ({}x{}), "
			   "scale factor {}",
			   design_resolution_.x,
			   design_resolution_.y,
			   screen_size_.x,
			   screen_size_.y,
			   drawing_resolution_.x,
			   drawing_resolution_.y,
			   scale_factor_);

	if(render_texture_.id != 0) {
		UnloadRenderTexture(render_texture_);
	}

	render_texture_ =
		LoadRenderTexture(static_cast<int>(drawing_resolution_.x), static_cast<int>(drawing_resolution_.y));
	if(render_texture_.id == 0) [[unlikely]] {
		return error("failed to create render texture on screen size change");
	}
	SetTextureFilter(render_texture_.texture, TEXTURE_FILTER_POINT);

	log::debug("render texture created with id {}, size ({}x{})",
			   render_texture_.id,
			   render_texture_.texture.width,
			   render_texture_.texture.height);

	return true;
}

auto raylib_renderer::get_texture_size(const texture_handle texture) -> glm::vec2 {
	Texture2D rl_texture{};
	if(const auto err = resource_manager_.get_raylib_texture(texture).unwrap(rl_texture); err) [[unlikely]] {
		log::error("failed to get texture with id {}", texture);
		return {0.0F, 0.0F};
	}
	return {static_cast<float>(rl_texture.width), static_cast<float>(rl_texture.height)};
}

auto raylib_renderer::get_sprite_frame_size(const sprite_sheet_handle sheet, const std::string_view frame)
	-> glm::vec2 {
	sprite_sheet_frame f{};
	if(const auto err = resource_manager_.get_sprite_sheet_frame(sheet, frame).unwrap(f); err) [[unlikely]] {
		log::error("failed to get sprite sheet frame '{}' from sheet {}", frame, sheet);
		return {0.0F, 0.0F};
	}
	return f.source_size;
}

auto raylib_renderer::render_sprite(const sprite_sheet_handle sheet,
									const std::string_view frame,
									const glm::vec2 &center,
									const glm::vec2 &size,
									const float rotation) const -> void {
	sprite_sheet_frame f{};
	if(const auto err = resource_manager_.get_sprite_sheet_frame(sheet, frame).unwrap(f); err) [[unlikely]] {
		log::error("failed to get sprite sheet frame '{}', skipping sprite render", frame);
		return;
	}

	Texture2D rl_texture{};
	if(const auto err = resource_manager_.get_sprite_sheet_texture(sheet).unwrap(rl_texture); err) [[unlikely]] {
		log::error("failed to get sprite sheet texture for sheet {}, skipping sprite render", sheet);
		return;
	}

	const auto screen_center = to_screen(center);
	const auto source =
		Rectangle{.x = f.source_pos.x, .y = f.source_pos.y, .width = f.source_size.x, .height = f.source_size.y};
	const auto dest = Rectangle{.x = screen_center.x, .y = screen_center.y, .width = size.x, .height = size.y};
	const auto origin = Vector2{.x = size.x * 0.5F, .y = size.y * 0.5F};

	DrawTexturePro(rl_texture, source, dest, origin, rotation, WHITE);
}

auto raylib_renderer::render_label(const font_handle font,
								   const std::string &text,
								   const int &size,
								   const glm::vec4 &color,
								   const glm::vec2 &pivot_position,
								   const glm::vec2 &pivot_to_top_left,
								   const float rotation) const -> void {
	const auto screen_pivot = to_screen(pivot_position);

	auto rl_font = GetFontDefault();
	if(const auto font_to_use = font.is_valid() ? font : default_font_; font_to_use.is_valid()) {
		if(const auto err = resource_manager_.get_raylib_font(font_to_use).unwrap(rl_font); err) [[unlikely]] {
			log::error("failed to get font with id {}, rendering label with default font instead", font_to_use);
		}
	}

	const auto spacing = static_cast<float>(size) / static_cast<float>(rl_font.baseSize);

	DrawTextPro(rl_font,
				text.c_str(),
				{.x = screen_pivot.x, .y = screen_pivot.y},
				{.x = -pivot_to_top_left.x, .y = -pivot_to_top_left.y},
				rotation,
				static_cast<float>(size),
				spacing,
				color_from_glm(color));
}

auto raylib_renderer::render_quad(const glm::vec2 &p0,
								  const glm::vec2 &p1,
								  const glm::vec2 &p2,
								  const glm::vec2 &p3,
								  const glm::vec4 &color) const -> void {
	const auto screen_p0 = to_screen(p0);
	const auto screen_p1 = to_screen(p1);
	const auto screen_p2 = to_screen(p2);
	const auto screen_p3 = to_screen(p3);

	const auto ray_vec0 = Vector2{.x = screen_p0.x, .y = screen_p0.y};
	const auto ray_vec1 = Vector2{.x = screen_p1.x, .y = screen_p1.y};
	const auto ray_vec2 = Vector2{.x = screen_p2.x, .y = screen_p2.y};
	const auto ray_vec3 = Vector2{.x = screen_p3.x, .y = screen_p3.y};

	const auto ray_color = color_from_glm(color);

	DrawLineV(ray_vec0, ray_vec1, ray_color);
	DrawLineV(ray_vec1, ray_vec2, ray_color);
	DrawLineV(ray_vec2, ray_vec3, ray_color);
	DrawLineV(ray_vec3, ray_vec0, ray_color);
}

auto raylib_renderer::render_rect(const glm::vec2 &center,
								  const glm::vec2 &size,
								  const float rotation,
								  const glm::vec4 &border_color,
								  const glm::vec4 &fill_color,
								  const float border_thickness) const -> void {
	const auto screen_center = to_screen(center);
	const auto fill = color_from_glm(fill_color);
	const auto border = color_from_glm(border_color);

	auto rec = Rectangle{.x = screen_center.x, .y = screen_center.y, .width = size.x, .height = size.y};
	DrawRectanglePro(rec, {.x = size.x * 0.5F, .y = size.y * 0.5F}, rotation, fill);

	if(border_color.a > 0.0F && border_thickness > 0.0F) {
		const auto rotation_rad = glm::radians(rotation);
		const auto cos_r = glm::cos(rotation_rad);
		const auto sin_r = glm::sin(rotation_rad);

		const auto half_border = border_thickness * 0.5F;
		const auto half_width = size.x * 0.5F;
		const auto half_height = size.y * 0.5F;

		const auto offset_top = glm::vec2{0.0F, -half_height + half_border};
		const auto offset_bottom = glm::vec2{0.0F, half_height - half_border};
		const auto offset_left = glm::vec2{-half_width + half_border, 0.0F};
		const auto offset_right = glm::vec2{half_width - half_border, 0.0F};

		const auto rotated_top =
			glm::vec2{(offset_top.x * cos_r) - (offset_top.y * sin_r), (offset_top.x * sin_r) + (offset_top.y * cos_r)};
		const auto rotated_bottom = glm::vec2{(offset_bottom.x * cos_r) - (offset_bottom.y * sin_r),
											  (offset_bottom.x * sin_r) + (offset_bottom.y * cos_r)};
		const auto rotated_left = glm::vec2{(offset_left.x * cos_r) - (offset_left.y * sin_r),
											(offset_left.x * sin_r) + (offset_left.y * cos_r)};
		const auto rotated_right = glm::vec2{(offset_right.x * cos_r) - (offset_right.y * sin_r),
											 (offset_right.x * sin_r) + (offset_right.y * cos_r)};

		const auto top_center = screen_center + rotated_top;
		const auto bottom_center = screen_center + rotated_bottom;
		const auto left_center = screen_center + rotated_left;
		const auto right_center = screen_center + rotated_right;

		const auto top_rect =
			Rectangle{.x = top_center.x, .y = top_center.y, .width = size.x, .height = border_thickness};
		const auto bottom_rect =
			Rectangle{.x = bottom_center.x, .y = bottom_center.y, .width = size.x, .height = border_thickness};
		const auto left_rect =
			Rectangle{.x = left_center.x, .y = left_center.y, .width = border_thickness, .height = size.y};
		const auto right_rect =
			Rectangle{.x = right_center.x, .y = right_center.y, .width = border_thickness, .height = size.y};

		DrawRectanglePro(top_rect, {.x = half_width, .y = half_border}, rotation, border);
		DrawRectanglePro(bottom_rect, {.x = half_width, .y = half_border}, rotation, border);
		DrawRectanglePro(left_rect, {.x = half_border, .y = half_height}, rotation, border);
		DrawRectanglePro(right_rect, {.x = half_border, .y = half_height}, rotation, border);
	}
}

auto raylib_renderer::render_circle(const glm::vec2 &center,
									const float radius,
									const glm::vec4 &border_color,
									const glm::vec4 &fill_color,
									const float border_thickness) const -> void {
	const auto screen_center = to_screen(center);
	const auto ray_border_color = color_from_glm(border_color);
	const auto ray_fill_color = color_from_glm(fill_color);
	const auto ray_center = Vector2{.x = screen_center.x, .y = screen_center.y};

	// has fill, draw the filled circle
	if(fill_color.a > 0.0F) {
		DrawCircleV({.x = screen_center.x, .y = screen_center.y}, radius, ray_fill_color);
	}

	// has border, draw the border
	if(border_color.a > 0.0F && border_thickness > 0.0F) {
		DrawRing(ray_center, radius - border_thickness, radius, 0.0F, 360.0F, 100, ray_border_color);
	}
}

} // namespace lge
