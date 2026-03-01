// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/app/app.hpp>
#include <lge/core/log.hpp>
#include <lge/core/result.hpp>
#include <lge/interface/backend.hpp>
#include <lge/internal/raylib/raylib_backend.hpp>
#include <lge/internal/systems/animation_system.hpp>
#include <lge/internal/systems/bounds_system.hpp>
#include <lge/internal/systems/collision_system.hpp>
#include <lge/internal/systems/hidden_system.hpp>
#include <lge/internal/systems/metrics_system.hpp>
#include <lge/internal/systems/order_system.hpp>
#include <lge/internal/systems/pointer_system.hpp>
#include <lge/internal/systems/render_system.hpp>
#include <lge/internal/systems/transform_system.hpp>
#include <lge/internal/systems/transition_system.hpp>
#include <lge/systems/system.hpp>

#include <memory>

#ifdef __EMSCRIPTEN__
#	include <emscripten/emscripten.h>
#endif

namespace lge {

app::app()
	: backend_{raylib_backend::create()},
	  ctx{
		  .render = *backend_.renderer_ptr,
		  .actions = *backend_.input_ptr,
		  .resources = *backend_.resource_manager_ptr,
		  .audio = *backend_.audio_manager_ptr,
		  .world = registry_,
		  .events = dispatcher_,
	  },
	  scenes{ctx} {}

auto app::run() -> result<> {
	if(const auto err = init().unwrap(); err) [[unlikely]] {
		return error("failed to init the application", *err);
	}

#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop_arg(
		[](void *arg) -> void {
			auto *self = static_cast<app *>(arg);
			if(const auto err = self->main_loop().unwrap(); err) [[unlikely]] {
				SPDLOG_ERROR("error main_loop: {}", err->get_message());
				emscripten_cancel_main_loop();
			}
		},
		this,
		0,
		true);
#else

#	ifdef NDEBUG
	backend_.renderer_ptr->set_fullscreen(true);
#	endif
	while(!should_exit_) {
		should_exit_ = should_exit_ || backend_.renderer_ptr->should_close();
		if(const auto err = main_loop().unwrap(); err) [[unlikely]] {
			return error("error during main loop", *err);
		}
	}

	if(const auto err = end().unwrap(); err) [[unlikely]] {
		return error("error ending the application", *err);
	}

	log::info("application ended");
#endif
	return true;
}

auto app::update(float /*dt*/) -> result<> {
	return true;
}

auto app::init() -> result<> {
	log::init();

	if(const auto err = backend_.renderer_ptr->init(configure()).unwrap(); err) [[unlikely]] {
		return error("failed to initialize renderer", *err);
	}

	if(const auto err = backend_.resource_manager_ptr->init().unwrap(); err) [[unlikely]] {
		return error("failed to initialize resource manager", *err);
	}

	if(const auto err = backend_.audio_manager_ptr->init().unwrap(); err) [[unlikely]] {
		return error("failed to initialize audio manager", *err);
	}

	if(const auto err = register_system<metrics_system>(phase::local_update).unwrap(); err) [[unlikely]] {
		return error("failed to register metrics_system", *err);
	}
	if(const auto err = register_system<animation_system>(phase::game_update).unwrap(); err) [[unlikely]] {
		return error("failed to register animation_system", *err);
	}
	if(const auto err = register_system<hidden_system>(phase::game_update).unwrap(); err) [[unlikely]] {
		return error("failed to register hidden_system", *err);
	}
	if(const auto err = register_system<transform_system>(phase::global_update).unwrap(); err) [[unlikely]] {
		return error("failed to register transform_system", *err);
	}
	if(const auto err = register_system<bounds_system>(phase::global_update).unwrap(); err) [[unlikely]] {
		return error("failed to register bounds_system", *err);
	}
	if(const auto err = register_system<order_system>(phase::global_update).unwrap(); err) [[unlikely]] {
		return error("failed to register order_system", *err);
	}
	if(const auto err = register_system<collision_system>(phase::global_update).unwrap(); err) [[unlikely]] {
		return error("failed to register collision_system", *err);
	}
	if(const auto err = register_system<pointer_system>(phase::global_update).unwrap(); err) [[unlikely]] {
		return error("failed to register pointer_system", *err);
	}
	if(const auto err = register_system<render_system>(phase::render).unwrap(); err) [[unlikely]] {
		return error("failed to register render_system", *err);
	}
	if(const auto err = register_system<transition_system>(phase::post_render, scenes).unwrap(); err) [[unlikely]] {
		return error("failed to register transition_system", *err);
	}

	log::info("application initialized successfully");

	return true;
}

auto app::end() -> result<> {
	if(const auto err = backend_.audio_manager_ptr->end().unwrap(); err) [[unlikely]] {
		return error("failed to shutdown audio manager", *err);
	}

	if(const auto err = backend_.resource_manager_ptr->end().unwrap(); err) [[unlikely]] {
		return error("failed to shutdown resource manager", *err);
	}

	if(const auto err = backend_.renderer_ptr->end().unwrap(); err) [[unlikely]] {
		return error("failed to shutdown renderer", *err);
	}

	for(const auto &system: systems_) {
		if(const auto err = system->end().unwrap(); err) [[unlikely]] {
			return error("failed to end system", *err);
		}
	}

	if(const auto err = scenes.end().unwrap(); err) [[unlikely]] {
		return error("failed to end scenes", *err);
	}

	return true;
}

auto app::main_loop() -> result<> {
	if(const auto err = backend_.renderer_ptr->begin_frame().unwrap(); err) [[unlikely]] {
		return error("failed to begin frame", *err);
	}

	const auto delta_time = backend_.renderer_ptr->get_delta_time();

	backend_.input_ptr->update(delta_time);

	backend_.renderer_ptr->show_cursor(!backend_.input_ptr->is_controller_available());

	if(const auto err = update(delta_time).unwrap(); err) [[unlikely]] {
		return error("failed to update the application", *err);
	}

	if(const auto err = update_system(phase::game_update, delta_time).unwrap(); err) [[unlikely]] {
		return error("failed to update systems in game update phase", *err);
	}

	if(const auto err = scenes.update(delta_time).unwrap(); err) [[unlikely]] {
		return error("failed to update scenes", *err);
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

	if(const auto err = backend_.renderer_ptr->end_frame().unwrap(); err) [[unlikely]] {
		return error("failed to end frame", *err);
	}

	if(const auto err = backend_.audio_manager_ptr->update_music().unwrap(); err) [[unlikely]] {
		return error("failed to update music", *err);
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