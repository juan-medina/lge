// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "animation_system.hpp"

#include <lge/components/sprite.hpp>
#include <lge/components/sprite_animation.hpp>
#include <lge/core/log.hpp>
#include <lge/core/result.hpp>
#include <lge/interface/resource_manager.hpp>

#include <cstddef>
#include <entity/fwd.hpp>
#include <entt/entt.hpp>

namespace lge {

auto animation_system::advance_animation(const entt::entity entity, sprite_animation &anim, const float dt) -> void {
	animation_library_anim clip{};
	if(const auto err = resource_manager_.get_animation(anim.handle, anim.name).unwrap(clip); err) [[unlikely]] {
		log::error("animation clip '{}' not found, skipping", anim.name);
		return;
	}

	anim.elapsed += dt;
	const auto frame_duration = 1.F / clip.fps;
	if(anim.elapsed < frame_duration) [[likely]] {
		return;
	}

	anim.elapsed -= frame_duration;
	anim.current_frame = (anim.current_frame + 1) % static_cast<int>(clip.frames.size());

	sprite_sheet_handle sheet{};
	if(const auto err = resource_manager_.get_animation_sprite_sheet(anim.handle).unwrap(sheet); err) [[unlikely]] {
		log::error("failed to get sprite sheet for animation library, skipping");
		return;
	}

	auto &spr = world.get_or_emplace<sprite>(entity);
	spr.sheet = sheet;
	spr.frame = clip.frames[static_cast<std::size_t>(anim.current_frame)];
}

auto animation_system::update(const float dt) -> result<> {
	for(const auto entity: world.view<sprite_animation>()) {
		auto &anim = world.get<sprite_animation>(entity);
		advance_animation(entity, anim, dt);
	}
	return true;
}

} // namespace lge