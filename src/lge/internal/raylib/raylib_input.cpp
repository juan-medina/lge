// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "raylib_input.hpp"

#include <lge/core/log.hpp>

#include <raylib.h>

#include <array>
#include <cstdlib>

namespace lge {

auto raylib_input::update(const float /*delta_time*/) -> void {
	update_controller_mode();
	reset_states();

	const auto stick = update_virtual_dpad_states();

	for(std::size_t bid = 0; bid < max_actions; ++bid) {
		auto &st      = states[bid];
		auto &binding = bindings[bid];
		accumulate_key_states(st, binding);
		if(controller_available && default_controller >= 0) {
			accumulate_button_states(st, binding, stick);
		}
	}

	prev_stick_dpad_ = stick;
}

auto raylib_input::accumulate_key_states(state &st, const binding &b) const -> void {
	for(const auto k: b.keys) {
		const auto rk = key_to_raylib(k);
		st.pressed  = st.pressed  || IsKeyPressed(rk);
		st.released = st.released || IsKeyReleased(rk);
		st.down     = st.down     || IsKeyDown(rk);
	}
}

auto raylib_input::accumulate_button_states(state &st, const binding &b, const std::array<bool, 4> &stick) const -> void {
	for(const auto btn: b.buttons) {
		const auto rb = button_to_raylib(btn);
		st.pressed  = st.pressed  || IsGamepadButtonPressed(default_controller, rb);
		st.released = st.released || IsGamepadButtonReleased(default_controller, rb);
		st.down     = st.down     || IsGamepadButtonDown(default_controller, rb);
		apply_stick_dpad(st, btn, stick);
	}
}

auto raylib_input::apply_stick_dpad(state &st, const button b, const std::array<bool, 4> &stick) const -> void {
	const auto idx = stick_dpad_index(b);
	if(idx < 0) {
		return;
	}
	const auto cur  = stick[idx];
	const auto prev = prev_stick_dpad_[idx];
	st.pressed  = st.pressed  || (cur && !prev);
	st.released = st.released || (!cur && prev);
	st.down     = st.down     || cur;
}

auto raylib_input::stick_dpad_index(const button b) -> int {
	switch(b) {
	case button::left_face_left:
		return 0;
	case button::left_face_right:
		return 1;
	case button::left_face_up:
		return 2;
	case button::left_face_down:
		return 3;
	default:
		return -1;
	}
}

auto raylib_input::update_virtual_dpad_states() const -> std::array<bool, 4> {
	std::array<bool, 4> stick{};
	if(controller_available && default_controller >= 0) {
		const auto xl = GetGamepadAxisMovement(default_controller, GAMEPAD_AXIS_LEFT_X);
		const auto yl = GetGamepadAxisMovement(default_controller, GAMEPAD_AXIS_LEFT_Y);
		const auto xr = GetGamepadAxisMovement(default_controller, GAMEPAD_AXIS_RIGHT_X);
		const auto yr = GetGamepadAxisMovement(default_controller, GAMEPAD_AXIS_RIGHT_Y);

		stick[0] = xl < -controller_axis_dead_zone || xr < -controller_axis_dead_zone;
		stick[1] = xl >  controller_axis_dead_zone || xr >  controller_axis_dead_zone;
		stick[2] = yl < -controller_axis_dead_zone || yr < -controller_axis_dead_zone;
		stick[3] = yl >  controller_axis_dead_zone || yr >  controller_axis_dead_zone;
	}
	return stick;
}

auto raylib_input::update_controller_mode() -> void {
	const auto was_no_controller = default_controller == -1;
	default_controller = -1;

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
					log::debug("validated controller: {}", name_str);
				}
			}

			if(!is_validated) {
				continue;
			}
		}
#endif
		default_controller = i;
		break;
	}

	if(default_controller != -1 && was_no_controller) {
		log::info("using controller: {}", GetGamepadName(default_controller));
		log::info("controller has {} axis", GetGamepadAxisCount(default_controller));
		controller_available = true;
	}

	if(default_controller == -1 || !IsGamepadAvailable(default_controller)) {
		controller_available = false;
		if(!was_no_controller) {
			log::info("controller disconnected");
		}
		return;
	}

	if(is_gamepad_input_detected()) {
		controller_available = true;
	}

	if(is_mouse_keyboard_active()) {
		controller_available = false;
	}
}

auto raylib_input::is_gamepad_input_detected() const -> bool {
	if(!IsGamepadAvailable(default_controller)) {
		return false;
	}

	for(int button = 0; button <= GAMEPAD_BUTTON_RIGHT_THUMB; button++) {
		if(IsGamepadButtonPressed(default_controller, button)) {
			return true;
		}
	}

	constexpr float axis_threshold = 0.3F;
	for(int axis = 0; axis <= GAMEPAD_AXIS_RIGHT_Y; axis++) {
		if(std::abs(GetGamepadAxisMovement(default_controller, axis)) > axis_threshold) {
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