// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <raylib.h>

#include <string_view>
#include <unordered_map>
#include <vector>

namespace lge {

class input {
public:
	using id = std::string_view;

	struct state {
		bool pressed = false;  // went down this frame
		bool released = false; // went up this frame
		bool down = false;	   // currently held
	};

	struct binding {
		std::vector<KeyboardKey> keys;
		std::vector<GamepadButton> buttons;
	};

	void bind(const id bid, const binding &binding) {
		bindings_[bid] = binding;
	}

	auto update(float delta_time) -> void;
	[[nodiscard]] auto get(id bid) const -> const state &;

	[[nodiscard]] auto is_controller_available() const -> bool {
		return controller_available_;
	}

private:
	std::unordered_map<id, binding> bindings_;
	std::unordered_map<id, state> states_;

	bool controller_available_{false};
	float mouse_inactive_time_{0.0F};
	float controller_inactive_time_{0.0F};
	int default_controller_{-1};

	static constexpr float controller_mode_grace_period = 2.0F;
	static constexpr auto controller_axis_dead_zone = 0.3F;

#ifdef __EMSCRIPTEN__
	std::unordered_set<std::string> validated_controllers_;
#endif

	[[nodiscard]] auto is_gamepad_input_detected() const -> bool;
	[[nodiscard]] static auto is_mouse_keyboard_active() -> bool;
	auto update_controller_mode(float delta_time) -> void;

	auto reset_states() -> void;
};

} // namespace lge
