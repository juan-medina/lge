// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "lge/renderer.hpp"

#include <raylib.h>

#include <cstdio>
#include <spdlog/spdlog.h>
#include <vector>

namespace lge {

auto renderer::init() -> result<> {
#if defined(_WIN32) || defined(__EMSCRIPTEN__)
	InitWindow(1920, 1080, "LGE Application");
#else
	InitWindow(0, 0, "LGE Application");
#endif

#ifndef __EMSCRIPTEN__
	if(!IsWindowReady()) {
		return error("failed to initialize window");
	}
#endif

	SetTargetFPS(60);

	initialized_ = true;

	SPDLOG_INFO("renderer initialized");
	return true;
}

auto renderer::end() -> result<> {
	if(!initialized_) {
		return true;
	}

	CloseWindow();
	initialized_ = false;

	SPDLOG_INFO("renderer ended");
	return true;
}

auto renderer::begin_frame() const -> result<> {
	if(!initialized_) {
		return error("renderer not initialized");
	}

	BeginDrawing();
	ClearBackground(BLACK);

	return true;
}

auto renderer::end_frame() const -> result<> {
	if(!initialized_) {
		return error("renderer not initialized");
	}

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
		SPDLOG_ERROR("[raylib] log formatting error in log callback");
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

	spdlog::log(level, "[raylib] {}", buffer.data());
}

} // namespace lge
