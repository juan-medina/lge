// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "lge/app.hpp"

#include <lge/result.hpp>

#include <optional>
#include <spdlog/common.h>
#include <spdlog/spdlog.h>

#ifdef __EMSCRIPTEN__
#	include <emscripten/emscripten.h>
#endif

namespace lge {

auto app::run() -> result<> {
	if(const auto err = init().unwrap(); err) [[unlikely]] {
		return error("failed to init the application", *err);
	}

#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop_arg(
		[](void *arg) -> void {
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
	renderer::set_fullscreen(true);
	while(!should_exit_) {
		should_exit_ = should_exit_ || renderer_.should_close();
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

	if(const auto err = renderer_.init().unwrap(); err) {
		return error("failed to initialize renderer", *err);
	}

	SPDLOG_INFO("application initialized successfully");

	return true;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
auto app::setup_log() -> result<> { // NOLINT(*-convert-member-functions-to-static)
#ifdef NDEBUG
	spdlog::set_level(spdlog::level::err);
#else
	spdlog::set_level(spdlog::level::debug);
#endif
	spdlog::set_pattern(empty_format);
	SPDLOG_INFO(banner);
	spdlog::set_pattern(color_line_format);

	renderer::setup_raylib_log();

	SPDLOG_INFO("log setup complete");
	return true;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
auto app::end() -> result<> { // NOLINT(*-convert-member-functions-to-static)
	if(const auto err = renderer_.end().unwrap(); err) {
		return error("failed to shutdown renderer", *err);
	}

	return true;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
auto app::main_loop() const -> result<> { // NOLINT(*-convert-member-functions-to-static)
	if(const auto err = renderer_.begin_frame().unwrap(); err) {
		return error("failed to begin frame", *err);
	}

	if(const auto err = renderer_.end_frame().unwrap(); err) {
		return error("failed to end frame", *err);
	}

	return true;
}

} // namespace lge
