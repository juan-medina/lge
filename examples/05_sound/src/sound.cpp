// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "sound.hpp"

#include <lge/app/main.hpp>
#include <lge/components/clickable.hpp>
#include <lge/components/hovered.hpp>
#include <lge/components/placement.hpp>
#include <lge/components/sprite.hpp>
#include <lge/components/sprite_animation.hpp>
#include <lge/core/colors.hpp>
#include <lge/core/result.hpp>
#include <lge/events/click.hpp>
#include <lge/interface/resource_manager.hpp>
#include <lge/interface/resources.hpp>

#include "../../src/example.hpp"

#include <cassert>
#include <cstddef>
#include <entt/core/fwd.hpp>
#include <entt/entt.hpp>
#include <random>

LGE_MAIN(examples::sound);

namespace examples {

auto sound::init() -> lge::result<> {
	if(const auto err = example::init().unwrap(); err) [[unlikely]] {
		return lge::error("error init the app", *err);
	}

	ctx.actions.bind(play_action,
					 {
						 .keys = {lge::input::key::space},
						 .buttons = {lge::input::button::right_face_down},
					 });

	if(const auto err = ctx.resources.load_animation_library(anim_path).unwrap(animation_library_); err) [[unlikely]] {
		return lge::error("failed to load animation library", *err);
	}

	if(const auto err = ctx.resources.load_sound(scratch_path).unwrap(scratch_sound_); err) [[unlikely]] {
		return lge::error("failed to load scratch sound", *err);
	}

	if(const auto err = ctx.resources.load_music(music_path).unwrap(music_); err) [[unlikely]] {
		return lge::error("failed to load music", *err);
	}

	sprite_ = ctx.world.create();
	ctx.world.emplace<lge::sprite_animation>(sprite_, animation_library_, idle_anim);
	ctx.world.emplace<lge::placement>(sprite_, 0.0F, 0.0F);
	ctx.world.emplace<lge::clickable>(sprite_);

	ctx.events.on<lge::click>([this](const lge::click &c) -> lge::result<> {
		if(c.entity != sprite_) {
			return true;
		}
		return toggle_music();
	});

	return true;
}

auto sound::update(const float dt) -> lge::result<> {
	if(ctx.actions.get(play_action).pressed) {
		if(const auto err = toggle_music().unwrap(); err) [[unlikely]] {
			return lge::error("failed to toggle music", *err);
		}
	}

	// when the sprite is on the game
	if(ctx.world.all_of<lge::sprite>(sprite_)) {
		auto &s = ctx.world.get<lge::sprite>(sprite_);
		auto sc = lge::colors::white;
		// if the controller is not available, we want to show the hovered state on the sprite when the mouse is over it
		if(!ctx.actions.is_controller_available()) {
			sc = ctx.world.all_of<lge::hovered>(sprite_) ? lge::colors::white : lge::colors::gray_70;
		}
		s.tint = sc;
	}

	return example::update(dt);
}

auto sound::end() -> lge::result<> {
	if(const auto err = ctx.resources.unload_music(music_).unwrap(); err) [[unlikely]] {
		return lge::error("failed to unload music", *err);
	}

	if(const auto err = ctx.resources.unload_sound(scratch_sound_).unwrap(); err) [[unlikely]] {
		return lge::error("failed to unload scratch sound", *err);
	}

	if(const auto err = ctx.resources.unload_animation_library(animation_library_).unwrap(); err) [[unlikely]] {
		return lge::error("failed to unload animation library", *err);
	}

	return example::end();
}

auto sound::toggle_music() -> lge::result<> {
	if(!ctx.audio.is_music_playing()) {
		ctx.world.replace<lge::sprite_animation>(sprite_, animation_library_, get_dance());
		if(const auto err = ctx.audio.play_music(music_).unwrap(); err) [[unlikely]] {
			return lge::error("failed to play music", *err);
		}
	} else {
		ctx.world.replace<lge::sprite_animation>(sprite_, animation_library_, idle_anim);
		if(const auto err = ctx.audio.stop_music().unwrap(); err) [[unlikely]] {
			return lge::error("failed to stop music", *err);
		}
		if(const auto err = ctx.audio.play_sound(scratch_sound_).unwrap(); err) [[unlikely]] {
			return lge::error("failed to play scratch sound", *err);
		}
	}
	return true;
}

auto sound::get_dance() -> entt::hashed_string {
	return dances[std::uniform_int_distribution<size_t>{0, dances.size() - 1}(rng_)];
}

} // namespace examples