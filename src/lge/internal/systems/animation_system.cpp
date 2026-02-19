// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "animation_system.hpp"

#include <lge/components/sprite.hpp>
#include <lge/components/sprite_animation.hpp>
#include <lge/core/result.hpp>

#include <cstddef>
#include <entity/fwd.hpp>
#include <entt/entt.hpp>

namespace lge {

auto animation_system::advance_frame(sprite_animation &anim, const float dt) -> bool {
	anim.elapsed += dt;
	const auto frame_duration = 1.F / anim.fps;
	if(anim.elapsed < frame_duration) [[likely]] {
		return false;
	}
	anim.elapsed -= frame_duration;
	anim.current_frame = (anim.current_frame + 1) % static_cast<int>(anim.frames.size());
	return true;
}

auto animation_system::update(const float dt) -> result<> {
	for(const auto entity: world.view<sprite_animation>()) {
		auto &anim = world.get<sprite_animation>(entity);
		if(!advance_frame(anim, dt)) [[likely]] {
			continue;
		}
		auto &spr = world.get_or_emplace<sprite>(entity);
		spr.sheet = anim.sheet;
		spr.frame = anim.frames[static_cast<std::size_t>(anim.current_frame)];
	}
	return true;
}

} // namespace lge