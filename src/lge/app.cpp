// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "lge/app.hpp"

#include <lge/result.hpp>

#include <raylib.h>

#include <spdlog/spdlog.h>

namespace lge {

auto app::run() -> result<> {
	if(const auto err = init().unwrap(); err) [[unlikely]] {
		return error("failed to init the application", *err);
	}

#ifdef _WIN32
	InitWindow(1920, 1080, "LGE Application");
#else
	InitWindow(0, 0, "LGE Application");
#endif

#ifndef __EMSCRIPTEN__
#	ifdef _WIN32
	ToggleBorderlessWindowed();
#	else
	ToggleFullscreen();
#	endif

#endif
#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop_arg(
		[](void *arg) {
			auto *self = static_cast<app *>(arg);
			if(const auto err = self->main_loop().unwrap(); err) {
				SPDLOG_ERROR("error main_loop: {}", err->get_message());
				emscripten_cancel_main_loop();
			}
		},
		this,
		0,
		true);
#else
	while(!should_exit_) {
		should_exit_ = should_exit_ || WindowShouldClose();
		if(const auto err = main_loop().unwrap(); err) {
			return error("error during main loop", *err);
		}
	}

	if(const auto err = end().unwrap(); err) {
		return error("error ending the application", *err);
	}

	SPDLOG_INFO("application ended");
#endif
	return true;
}

auto app::init() -> result<> {
	if(const auto err = setup_log().unwrap(); err) [[unlikely]] {
		return error("failed to setup log", *err);
	}

	SPDLOG_INFO("application initialized successfully");

	return true;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
auto app::setup_log() -> result<> { // NOLINT(*-convert-member-functions-to-static)
#ifdef NDEBUG
	spdlog::set_level(spdlog::level::err);
	SetTraceLogLevel(LOG_ERROR);
#else
	spdlog::set_level(spdlog::level::debug);
	SetTraceLogLevel(LOG_DEBUG);
#endif
	spdlog::set_pattern(empty_format);
	SPDLOG_INFO(banner);
	spdlog::set_pattern(color_line_format);

	SetTraceLogCallback(log_callback);

	SPDLOG_INFO("log setup complete");
	return true;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
auto app::end() -> result<> { // NOLINT(*-convert-member-functions-to-static)

	CloseWindow();

	return true;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
auto app::main_loop() -> result<> { // NOLINT(*-convert-member-functions-to-static)

	BeginDrawing();
	ClearBackground(BLACK);
	DrawText("Hello, LGE!", 10, 10, 20, RAYWHITE);
	EndDrawing();

	return true;
}

auto app::log_callback(const int log_level, const char *text, va_list args) -> void {
	constexpr std::size_t initial_size = 1024;
	thread_local std::vector<char> buffer(initial_size);

	va_list args_copy{}; // NOLINT(cppcoreguidelines-pro-type-vararg)
	va_copy(args_copy, args);
	int const needed = std::vsnprintf(buffer.data(), buffer.size(), text, args_copy);
	va_end(args_copy);

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
