// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "animation_system.hpp"

#include <lge/components/sprite.hpp>
#include <lge/components/sprite_animation.hpp>
#include <lge/core/log.hpp>
#include <lge/core/result.hpp>
#include <lge/interface/resource_manager.hpp>
#include <lge/internal/components/previous_sprite_animation.hpp>

#include <cstddef>
#include <entity/fwd.hpp>
#include <entt/entt.hpp>

namespace lge {

auto animation_system::advance_animation(const entt::entity entity, sprite_animation &anim, const float dt) -> void {
	auto &spr = world.get_or_emplace<sprite>(entity);
	auto &previous_anim = world.get_or_emplace<previous_sprite_animation>(entity);
	const auto anim_library_changed = previous_anim.handle != anim.handle;
	const auto anim_named_changed = previous_anim.name != anim.name;

	// We need to update the sprite's flip state every frame, since the animation clip may have changed but not the
	//  library or name, and the flip state is shared across all clips in the library
	spr.flip_horizontal = anim.flip_horizontal;
	spr.flip_vertical = anim.flip_vertical;

	if(anim_library_changed || anim_named_changed) [[likely]] {
		// Animation has changed, so we need to reset it
		anim.current_frame = 0;
		anim.elapsed = 0.F;
		previous_anim.handle = anim.handle;
		previous_anim.name = anim.name;

		// We need to update the sprite sheet only if we change the animation library, since the sprite sheet is
		//  shared across all animations in the library
		if(anim_library_changed) [[unlikely]] {
			if(const auto err = resource_manager_.get_animation_sprite_sheet(anim.handle).unwrap(spr.sheet); err)
				[[unlikely]] {
				log::error("failed to get sprite sheet for animation library, skipping");
				return;
			}
		}
	}

	animation_library_anim clip{};
	if(const auto err = resource_manager_.get_animation(anim.handle, anim.name).unwrap(clip); err) [[unlikely]] {
		log::error("animation clip '{}' not found, skipping", anim.name);
		return;
	}

	anim.elapsed += dt;
	const auto frame_duration = 1.F / clip.fps;
	if(anim.elapsed >= frame_duration) [[unlikely]] {
		anim.elapsed -= frame_duration;
		anim.current_frame = (anim.current_frame + 1) % static_cast<int>(clip.frames.size());
	}
	spr.frame = clip.frames[static_cast<std::size_t>(anim.current_frame)];;

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