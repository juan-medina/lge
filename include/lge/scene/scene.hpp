// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/app/context.hpp>
#include <lge/components/clear_on_scene_exit.hpp>
#include <lge/core/log.hpp>
#include <lge/core/result.hpp>
#include <lge/core/types.hpp>
#include <lge/systems/system.hpp>

#include <entt/entity/fwd.hpp>
#include <memory>
#include <vector>

namespace lge {

class scene {
public:
	explicit scene(context &the_context): ctx{the_context} {}
	virtual ~scene() = default;

	scene(const scene &) = delete;
	scene(scene &&) = delete;
	auto operator=(const scene &) -> scene & = delete;
	auto operator=(scene &&) -> scene & = delete;

	// Called by scene_manager — non-virtual, enforces system execution order
	[[nodiscard]] auto tick(float dt) -> result<>;

	[[nodiscard]] virtual auto end() -> result<>;

	[[nodiscard]] virtual auto on_pause() -> result<> {
		return true;
	}
	[[nodiscard]] virtual auto on_resume() -> result<> {
		return true;
	}
	[[nodiscard]] virtual auto on_exit() -> result<> {
		return true;
	}

	// Overridden by game scenes — called by tick() after systems
	[[nodiscard]] virtual auto update(float) -> result<> {
		return true;
	}

	auto clear_scene_entities() -> void {
		for(const auto view = ctx.world.view<clear_on_scene_exit>();
			const auto e: std::vector<entt::entity>{view.begin(), view.end()}) {
			if(ctx.world.valid(e)) [[likely]] {
				ctx.world.destroy(e);
			}
		}
	}

	// =============================================================================
	// System registration
	// =============================================================================

	template<typename System_T, typename... Args>
		requires std::is_base_of_v<system, System_T>
	[[nodiscard]] auto register_system(Args &&...args) -> result<> {
		auto sys = std::make_unique<System_T>(phase::game_update, ctx, std::forward<Args>(args)...);
		if(const auto err = sys->init().unwrap(); err) [[unlikely]] {
			return error("failed to init system", *err);
		}
		const auto type_name = get_type_name<System_T>();
		systems_.push_back(std::move(sys));
		log::debug("scene system of type `{}` registered", type_name);
		return true;
	}

protected:
	context &ctx;

private:
	[[nodiscard]] auto update_systems(float dt) -> result<>;

	std::vector<std::unique_ptr<system>> systems_;
};

} // namespace lge
