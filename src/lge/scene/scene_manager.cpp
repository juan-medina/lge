// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/core/result.hpp>
#include <lge/scene/scene_manager.hpp>

#include <algorithm>
#include <cmath>
#include <format>

namespace lge {

auto scene_manager::init() -> result<> {
	return true;
}

auto scene_manager::end() -> result<> {
	for(auto &[key, scene_ptr]: scenes_) {
		if(const auto err = scene_ptr->end().unwrap(); err) [[unlikely]] {
			return error(std::format("error ending scene with id `{}`", key), *err);
		}
	}
	scenes_.clear();
	current_scene_.reset();
	return true;
}

auto scene_manager::update(const float dt) -> result<> {
	switch(state_) {
	case transition_state::idle: {
		if(!current_scene_.has_value()) {
			return true;
		}
		return current_scene_->get().update(dt);
	}

	case transition_state::fade_out: {
		transition_elapsed_ += dt;
		const auto t = std::clamp(transition_elapsed_ / transition_duration_, 0.0F, 1.0F);
		overlay_alpha_ = static_cast<uint8_t>(std::lround(t * 255.0F));

		if(t >= 1.0F) {
			// Perform the scene swap.
			if(pending_activate_) {
				if(const auto err = pending_activate_().unwrap(); err) [[unlikely]] {
					return error("error during transition scene swap", *err);
				}
				pending_activate_ = nullptr;
			}
			transition_elapsed_ = 0.0F;
			state_ = transition_state::fade_in;
			log::debug("transition: scene swapped, starting fade-in");
		}
		return true;
	}

	case transition_state::fade_in: {
		transition_elapsed_ += dt;
		const auto t = std::clamp(transition_elapsed_ / transition_duration_, 0.0F, 1.0F);
		overlay_alpha_ = static_cast<uint8_t>(std::lround((1.0F - t) * 255.0F));

		if(t >= 1.0F) {
			overlay_alpha_ = 0;
			state_ = transition_state::idle;
			// Resume new scene now that it is fully visible.
			if(current_scene_.has_value()) {
				if(const auto err = current_scene_->get().on_resume().unwrap(); err) [[unlikely]] {
					return error("error resuming scene after transition", *err);
				}
			}
			log::debug("transition complete");
		}
		return true;
	}
	}

	return true; // unreachable — silence compiler
}

} // namespace lge