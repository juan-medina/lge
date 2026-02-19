// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/interface/resource_manager.hpp>

#include <string>
#include <string_view>
#include <vector>

namespace lge {

struct sprite_animation {
	sprite_sheet_handle sheet;
	std::vector<std::string> frames;
	float fps;
	int current_frame = 0;
	float elapsed = 0.F;
};

[[nodiscard]] auto frame(std::string_view prefix, int number, std::string_view suffix = ".png") -> std::string;

[[nodiscard]] auto make_frames(std::string_view prefix, int start, int count, std::string_view suffix = ".png")
	-> std::vector<std::string>;

[[nodiscard]] auto make_frames(std::string_view prefix, int count, std::string_view suffix = ".png")
	-> std::vector<std::string>;

auto play(sprite_animation &anim, std::vector<std::string> frames, float fps = 0) -> void;

} // namespace lge