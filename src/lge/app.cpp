// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "lge/app.hpp"

#include <lge/log.hpp>
#include <lge/raylib/raylib_input.hpp>
#include <lge/raylib/raylib_renderer.hpp>
#include <lge/result.hpp>
#include <lge/systems/bounds_system.hpp>
#include <lge/systems/hidden_system.hpp>
#include <lge/systems/metrics_system.hpp>
#include <lge/systems/render_system.hpp>
#include <lge/systems/system.hpp>
#include <lge/systems/transform_system.hpp>

#include <memory>
#include <optional>
#include <spdlog/common.h>
#include <spdlog/spdlog.h>

#ifdef __EMSCRIPTEN__
#	include <emscripten/emscripten.h>
#endif

namespace lge {

app::app() {
	renderer_ = std::make_unique<raylib_renderer>();
	input_ = std::make_unique<raylib_input>();
}

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

#	ifdef NDEBUG
	renderer_->set_fullscreen(true);
#	endif
	while(!should_exit_) {
		should_exit_ = should_exit_ || renderer_->should_close();
		if(const auto err = main_loop().unwrap(); err) {
			return error("error during main loop", *err);
		}
	}

	if(const auto err = end().unwrap(); err) {
		return error("error ending the application", *err);
	}

	LGE_INFO("application ended");
#endif
	return true;
}

auto app::init() -> result<> {
	if(const auto err = setup_log().unwrap(); err) [[unlikely]] {
		return error("failed to setup log", *err);
	}

	if(const auto err = renderer_->init(configure()).unwrap(); err) [[unlikely]] {
		return error("failed to initialize renderer", *err);
	}

	// phase::game_update systems go here
	register_system<metrics_system>(phase::local_update, *renderer_);
	register_system<bounds_system>(phase::game_update);
	register_system<hidden_system>(phase::game_update);
	register_system<transform_system>(phase::global_update);
	register_system<render_system>(phase::render, *renderer_);
	// future post render systems go here

	LGE_INFO("application initialized successfully");

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
	LGE_INFO(banner);
	spdlog::set_pattern(color_line_format);

	LGE_INFO("log setup complete");
	return true;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
auto app::end() const -> result<> { // NOLINT(*-convert-member-functions-to-static)
	if(const auto err = renderer_->end().unwrap(); err) [[unlikely]] {
		return error("failed to shutdown renderer", *err);
	}

	return true;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
auto app::main_loop() -> result<> { // NOLINT(*-convert-member-functions-to-static)
	if(const auto err = renderer_->begin_frame().unwrap(); err) [[unlikely]] {
		return error("failed to begin frame", *err);
	}

	auto const delta_time = renderer_->get_delta_time();

	input_->update(delta_time);

	renderer_->show_cursor(!input_->is_controller_available());

	if(const auto err = update(delta_time).unwrap(); err) [[unlikely]] {
		return error("failed to update the application", *err);
	}

	if(const auto err = update_system(phase::game_update, delta_time).unwrap(); err) [[unlikely]] {
		return error("failed to update systems in local update phase", *err);
	}

	if(const auto err = update_system(phase::local_update, delta_time).unwrap(); err) [[unlikely]] {
		return error("failed to update systems in local update phase", *err);
	}

	if(const auto err = update_system(phase::global_update, delta_time).unwrap(); err) [[unlikely]] {
		return error("failed to update systems in global update phase", *err);
	}

	if(const auto err = update_system(phase::render, delta_time).unwrap(); err) [[unlikely]] {
		return error("failed to update systems in render phase", *err);
	}

	if(const auto err = update_system(phase::post_render, delta_time).unwrap(); err) [[unlikely]] {
		return error("failed to update systems in post render phase", *err);
	}

	if(const auto err = renderer_->end_frame().unwrap(); err) [[unlikely]] {
		return error("failed to end frame", *err);
	}

	return true;
}

auto app::update_system(const phase p, const float dt) const -> result<> {
	for(const auto &system: systems_) {
		if(system->get_phase() == p) {
			if(const auto err = system->update(dt).unwrap(); err) [[unlikely]] {
				return error("failed to update system", *err);
			}
		}
	}

	return true;
}

} // namespace lge
