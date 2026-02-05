// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/result.hpp>

#include <cstdarg>

namespace lge {

class app {
public:
	explicit app() = default;
	virtual ~app() = default;

	// Disable copying and moving — apps are not copyable or movable
	app(const app &) = delete;
	app(app &&) = delete;
	auto operator=(const app &) -> app & = delete;
	auto operator=(app &&) -> app & = delete;

	[[nodiscard]] auto run() -> result<>;

protected:
	virtual auto init() -> result<>;

private:
	auto setup_log() -> result<>;
	auto end() -> result<>;
	auto main_loop() -> result<>;
	static auto log_callback(int log_level, const char *text, va_list args) -> void;

	static constexpr auto empty_format = "%v";
	static constexpr auto color_line_format = "[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v %@";
	static constexpr auto banner = "\033[38;2;0;255;0ml\033[38;2;128;128;128m[\033[38;2;0;0;255mg\033[38;2;128;128;"
								   "128m]\033[38;2;255;0;0me\033[0m";

#ifndef __EMSCRIPTEN__
	bool should_exit_ = false;
#endif

	[[nodiscard]] static auto is_fullscreen() -> bool;
	static auto set_fullscreen(bool fullscreen) -> void;
	static auto toggle_fullscreen() -> void;
};

} // namespace lge
