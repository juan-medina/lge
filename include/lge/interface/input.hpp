// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace lge {

class input {
public:
	input() = default;
	virtual ~input() = default;

	// Disable copying and moving — input is not copyable or movable
	input(const input &) = delete;
	input(input &&) = delete;
	auto operator=(const input &) -> input & = delete;
	auto operator=(input &&) -> input & = delete;

	enum class key : std::uint8_t {
		null_key,
		apostrophe,
		comma,
		minus,
		period,
		slash,
		zero,
		one,
		two,
		three,
		four,
		five,
		six,
		seven,
		eight,
		nine,
		semicolon,
		equal,
		a,
		b,
		c,
		d,
		e,
		f,
		g,
		h,
		i,
		j,
		k,
		l,
		m,
		n,
		o,
		p,
		q,
		r,
		s,
		t,
		u,
		v,
		w,
		x,
		y,
		z,
		left_bracket,
		backslash,
		right_bracket,
		grave,
		space,
		escape,
		enter,
		tab,
		backspace,
		insert,
		del,
		right,
		left,
		down,
		up,
		page_up,
		page_down,
		home,
		end,
		caps_lock,
		scroll_lock,
		num_lock,
		print_screen,
		pause,
		f1,
		f2,
		f3,
		f4,
		f5,
		f6,
		f7,
		f8,
		f9,
		f10,
		f11,
		f12,
		left_shift,
		left_control,
		left_alt,
		left_super,
		right_shift,
		right_control,
		right_alt,
		right_super,
		kb_menu,
		kp_0,
		kp_1,
		kp_2,
		kp_3,
		kp_4,
		kp_5,
		kp_6,
		kp_7,
		kp_8,
		kp_9,
		kp_decimal,
		kp_divide,
		kp_multiply,
		kp_subtract,
		kp_add,
		kp_enter,
		kp_equal,
		back,
		menu,
		volume_up,
		volume_down
	};

	enum class button : std::uint8_t {
		unknown,
		left_face_up,
		left_face_right,
		left_face_down,
		left_face_left,
		right_face_up,
		right_face_right,
		right_face_down,
		right_face_left,
		left_trigger_1,
		left_trigger_2,
		right_trigger_1,
		right_trigger_2,
		middle_left,
		middle,
		middle_right,
		left_thumb,
		right_thumb,
	};

	using id = std::size_t;

	struct state {
		bool pressed = false;  // went down this frame
		bool released = false; // went up this frame
		bool down = false;	   // currently held
	};

	struct binding {
		std::vector<key> keys;
		std::vector<button> buttons;
	};

	static constexpr std::size_t max_actions = 16;

	void bind(const id bid, const binding &b) {
		assert(bid < max_actions && "action id exceeds max_actions");
		bindings[bid] = b;
	}

	[[nodiscard]] auto is_controller_available() const -> bool {
		return controller_available;
	}

	[[nodiscard]] virtual auto get(const id bid) const -> const state & {
		assert(bid < max_actions && "action id exceeds max_actions");
		return states[bid];
	}

	virtual auto update(float delta_time) -> void = 0;

protected:
	std::array<binding, max_actions> bindings{};
	std::array<state, max_actions> states{};

	bool controller_available{false};
	int default_controller{-1};

	auto reset_states() -> void {
		for(auto &st: states) {
			st.pressed = false;
			st.released = false;
			st.down = false;
		}
	}
};

} // namespace lge