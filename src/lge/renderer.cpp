// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/app_config.hpp>
#include <lge/log.hpp>
#include <lge/renderer.hpp>
#include <lge/result.hpp>

#include <raylib.h>

#include <cstdarg>
#include <cstdio>
#include <spdlog/spdlog.h>
#include <vector>

namespace lge {

auto renderer::init(const app_config &config) -> result<> {
	clear_color_ = ColorFromGLM(config.clear_color);
	design_resolution_ = config.design_resolution;
	title_ = config.window_title;

#if defined(_WIN32) || defined(__EMSCRIPTEN__)
	InitWindow(1920, 1080, title_.c_str());
#else
	InitWindow(0, 0, title_.c_str());
#endif

#ifndef __EMSCRIPTEN__
	if(!IsWindowReady()) {
		return error("failed to initialize window");
	}
#endif

	SetTargetFPS(60);

	initialized_ = true;

	LGE_INFO("renderer initialized");
	return true;
}

auto renderer::end() -> result<> {
	if(!initialized_) {
		return true;
	}

	CloseWindow();
	initialized_ = false;

	if(render_texture_.id != 0) {
		UnloadRenderTexture(render_texture_);
	}

	LGE_INFO("renderer ended");
	return true;
}

auto renderer::begin_frame() -> result<> {
	if(!initialized_) {
		return error("renderer not initialized");
	}

	if(auto const screen_size = glm::vec2{static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())};
	   screen_size_.x != screen_size.x || screen_size_.y != screen_size.y) {
		if(auto const err = screen_size_changed(screen_size).unwrap(); err) {
			return error("failed to handle screen size change: {}", *err);
		}
	}

	BeginTextureMode(render_texture_);
	ClearBackground(clear_color_);

	return true;
}

auto renderer::end_frame() const -> result<> {
	if(!initialized_) {
		return error("renderer not initialized");
	}

	EndTextureMode();

	BeginDrawing();
	ClearBackground(clear_color_);
	DrawTexturePro(render_texture_.texture,
				   {0.0F,
					0.0F,
					static_cast<float>(render_texture_.texture.width),
					-static_cast<float>(render_texture_.texture.height)},
				   {0.0F, 0.0F, screen_size_.x, screen_size_.y},
				   {0.0F, 0.0F},
				   0.0F,
				   WHITE);
	EndDrawing();

	return true;
}

auto renderer::should_close() const -> bool {
	if(!initialized_) {
		return false;
	}

	return WindowShouldClose();
}

auto renderer::is_fullscreen() -> bool {
	bool full_screen = false;
#ifdef _WIN32
	full_screen = IsWindowState(FLAG_BORDERLESS_WINDOWED_MODE);
#else
	full_screen = IsWindowFullscreen();
#endif
	return full_screen;
}

auto renderer::set_fullscreen(const bool fullscreen) -> void {
	if(const auto current_state = is_fullscreen(); current_state != fullscreen) {
		toggle_fullscreen();
	}
}

auto renderer::toggle_fullscreen() -> void {
#ifdef WIN32
	ToggleBorderlessWindowed();
#else
	ToggleFullscreen();
#endif
}

auto renderer::setup_raylib_log() -> void {
#ifdef NDEBUG
	SetTraceLogLevel(LOG_ERROR);
#else
	SetTraceLogLevel(LOG_DEBUG);
#endif
	SetTraceLogCallback(log_callback);
}

auto renderer::get_delta_time() -> float {
	return GetFrameTime();
}

auto renderer::log_callback(const int log_level, const char *text, va_list args) -> void {
	constexpr std::size_t initial_size = 1024;
	thread_local std::vector<char> buffer(initial_size);

	va_list args_copy{};	  // NOLINT(cppcoreguidelines-pro-type-vararg)
	va_copy(args_copy, args); // NOLINT(*-pro-bounds-array-to-pointer-decay)
	int const needed =
		std::vsnprintf(buffer.data(), buffer.size(), text, args_copy); // NOLINT(*-pro-bounds-array-to-pointer-decay)
	va_end(args_copy);												   // NOLINT(*-pro-bounds-array-to-pointer-decay)

	if(needed < 0) {
		LGE_INFO("[raylib] log formatting error in log callback");
		return;
	}

	if(static_cast<std::size_t>(needed) >= buffer.size()) {
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

	spdlog::log(level, "[raylib] " + std::string(buffer.data()));
}

auto renderer::screen_size_changed(const glm::vec2 screen_size) -> result<> {
	screen_size_ = screen_size;

	scale_factor_ = screen_size_.y / design_resolution_.y;
	drawing_resolution_.y = design_resolution_.y;
	drawing_resolution_.x = static_cast<float>(static_cast<int>(screen_size_.x / scale_factor_));

	LGE_DEBUG("display resized, design resolution ({},{}) real resolution ({}x{}), drawing resolution ({}x{}), "
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
	if(render_texture_.id == 0) {
		return error("failed to create render texture on screen size change");
	}
	SetTextureFilter(render_texture_.texture, TEXTURE_FILTER_POINT);

	LGE_DEBUG("render texture created with id {}, size ({}x{})", render_texture_.id, render_texture_.texture.width,
			  render_texture_.texture.height);

	return true;
}

auto renderer::render_label(const label &label, const glm::vec2 &position) const -> void {
	const auto text_width = static_cast<float>(MeasureText(label.text.c_str(), static_cast<int>(label.size)));
	const auto text_height = label.size;

	auto aligned = position;

	switch(label.horizontal_align) {
	case horizontal_alignment::center:
		aligned.x -= text_width * 0.5f;
		break;
	case horizontal_alignment::right:
		aligned.x -= text_width;
		break;
	case horizontal_alignment::left:
		break;
	}

	switch(label.vertical_align) {
	case vertical_alignment::center:
		aligned.y -= text_height * 0.5f;
		break;
	case vertical_alignment::bottom:
		aligned.y -= text_height;
		break;
	case vertical_alignment::top:
		break;
	}

	const auto screen_pos = to_screen(aligned);

	DrawText(label.text.c_str(),
			 static_cast<int>(screen_pos.x),
			 static_cast<int>(screen_pos.y),
			 static_cast<int>(label.size),
			 ColorFromGLM(label.color));
}

auto renderer::render_aabb(const global_aabb ga) const -> void {
	const auto screen_min = to_screen(ga.min);
	const auto screen_max = to_screen(ga.max);
	DrawRectangleLines(static_cast<int>(screen_min.x),
					   static_cast<int>(screen_min.y),
					   static_cast<int>(screen_max.x - screen_min.x),
					   static_cast<int>(screen_max.y - screen_min.y),
					   RED);
}

} // namespace lge
