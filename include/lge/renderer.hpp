// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/result.hpp>

namespace lge {

class renderer {
public:
	explicit renderer() = default;
	~renderer() = default;

	renderer(const renderer &) = delete;
	renderer(renderer &&) = delete;
	auto operator=(const renderer &) -> renderer & = delete;
	auto operator=(renderer &&) -> renderer & = delete;

	[[nodiscard]] auto init() -> result<>;
	[[nodiscard]] auto end() -> result<>;

	[[nodiscard]] auto begin_frame() const -> result<>;
	[[nodiscard]] auto end_frame() const -> result<>;

	[[nodiscard]] auto should_close() const -> bool;

	[[nodiscard]] static auto is_fullscreen() -> bool;
	static auto set_fullscreen(bool fullscreen) -> void;
	static auto toggle_fullscreen() -> void;

	static auto setup_raylib_log() -> void;

private:
	static auto log_callback(int log_level, const char *text, va_list args) -> void;

	bool initialized_ = false;
};

} // namespace lge
