// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "raylib_input.hpp"

#include <lge/log.hpp>

#include <raylib.h>

#include <cstdlib>
#include <unordered_map>

namespace lge {

auto raylib_input::update(const float delta_time) -> void {
	update_controller_mode(delta_time);
	reset_states();

	for(auto &[bid, binding]: bindings_) {
		auto &[pressed, released, down] = states_[bid];

		for(const auto k: binding.keys) {
			const auto raylib_key = key_to_raylib(k);
			pressed = pressed || IsKeyPressed(raylib_key);
			released = released || IsKeyReleased(raylib_key);
			down = down || IsKeyDown(raylib_key);
		}

		if(controller_available_ && default_controller_ >= 0) {
			for(const auto b: binding.buttons) {
				const auto raylib_button = button_to_raylib(b);
				pressed = pressed || IsGamepadButtonPressed(default_controller_, raylib_button);
				released = released || IsGamepadButtonReleased(default_controller_, raylib_button);
				down = down || IsGamepadButtonDown(default_controller_, raylib_button);
			}
		}
	}
}

auto raylib_input::update_controller_mode(const float delta_time) -> void {
	const auto was_no_controller = default_controller_ == -1;
	default_controller_ = -1;

	for(int i = 0; i < 4; ++i) {
		if(!IsGamepadAvailable(i)) {
			continue;
		}

#ifdef __EMSCRIPTEN__
		const char *name = GetGamepadName(i);
		if(name != nullptr) {
			std::string name_str(name);

			bool is_validated = validated_controllers_.contains(name_str);

			if(!is_validated) {
				bool raylib_input_detected = false;

				for(int button = 0; button <= GAMEPAD_BUTTON_RIGHT_THUMB && !raylib_input_detected; button++) {
					if(IsGamepadButtonPressed(i, button)) {
						raylib_input_detected = true;
					}
				}

				constexpr float axis_threshold = 0.3f;
				for(int axis = 0; axis <= GAMEPAD_AXIS_RIGHT_Y && !raylib_input_detected; axis++) {
					if(std::abs(GetGamepadAxisMovement(i, axis)) > axis_threshold) {
						raylib_input_detected = true;
					}
				}

				if(raylib_input_detected) {
					validated_controllers_.insert(name_str);
					is_validated = true;
					SPDLOG_DEBUG("validated controller: {}", name_str);
				}
			}

			if(!is_validated) {
				continue;
			}
		}
#endif
		default_controller_ = i;
		break;
	}

	if(default_controller_ != -1 && was_no_controller) {
		LGE_INFO("using controller: {}", GetGamepadName(default_controller_));
		LGE_INFO("controller has {} axis", GetGamepadAxisCount(default_controller_));
		controller_available_ = true;
	}

	if(default_controller_ == -1 || !IsGamepadAvailable(default_controller_)) {
		controller_available_ = false;
		mouse_inactive_time_ = 0.0F;
		controller_inactive_time_ = 0.0F;
		if(!was_no_controller) {
			LGE_INFO("controller disconnected");
		}
		return;
	}

	if(is_gamepad_input_detected()) {
		controller_inactive_time_ = 0.0F;
		// Only switch to controller mode if grace period expired or already in controller mode
		if(controller_available_ || mouse_inactive_time_ > controller_mode_grace_period) {
			controller_available_ = true;
		}
	}

	if(is_mouse_keyboard_active()) {
		mouse_inactive_time_ = 0.0F;
		// Only switch to mouse mode if grace period expired or already in mouse mode
		if(!controller_available_ || controller_inactive_time_ > controller_mode_grace_period) {
			controller_available_ = false;
		}
	}

	// Update inactive timers
	mouse_inactive_time_ += delta_time;
	controller_inactive_time_ += delta_time;

	// Switch modes after grace period of inactivity from current mode
	if(controller_available_ && controller_inactive_time_ > controller_mode_grace_period) {
		// If controller hasn't been used for a while, allow mouse to take over
		if(mouse_inactive_time_ < controller_inactive_time_) {
			controller_available_ = false;
		}
	} else if(!controller_available_ && mouse_inactive_time_ > controller_mode_grace_period) {
		// If mouse hasn't been used for a while, allow controller to take over
		if(controller_inactive_time_ < mouse_inactive_time_) {
			controller_available_ = true;
		}
	}
}

auto raylib_input::is_gamepad_input_detected() const -> bool {
	if(!IsGamepadAvailable(default_controller_)) {
		return false;
	}

	for(int button = 0; button <= GAMEPAD_BUTTON_RIGHT_THUMB; button++) {
		if(IsGamepadButtonPressed(default_controller_, button)) {
			return true;
		}
	}

	constexpr float axis_threshold = 0.3F;
	for(int axis = 0; axis <= GAMEPAD_AXIS_RIGHT_Y; axis++) {
		if(std::abs(GetGamepadAxisMovement(default_controller_, axis)) > axis_threshold) {
			return true;
		}
	}

	return false;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
auto raylib_input::is_mouse_keyboard_active() -> bool { // NOLINT(*-convert-member-functions-to-static)
	constexpr auto delta_threshold = 2.0F;
	const auto [mouse_delta_x, mouse_delta_y] = GetMouseDelta();

	return (std::abs(mouse_delta_x) > delta_threshold) || (std::abs(mouse_delta_y) > delta_threshold)
		   || IsMouseButtonDown(0) || IsMouseButtonDown(1) || IsMouseButtonDown(2) || GetKeyPressed() != 0;
}

} // namespace lge