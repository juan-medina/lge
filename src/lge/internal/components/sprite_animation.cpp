// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/components/sprite_animation.hpp>

#include <cstddef>
#include <format>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace lge {

auto frame(std::string_view prefix, int number, std::string_view suffix) -> std::string {
	return std::format("{}{}{}", prefix, number, suffix);
}

auto make_frames(const std::string_view prefix, const int start, const int count, const std::string_view suffix)
	-> std::vector<std::string> {
	std::vector<std::string> result;
	result.reserve(static_cast<std::size_t>(count));
	for(int i = start; i < start + count; ++i) {
		result.emplace_back(frame(prefix, i, suffix));
	}
	return result;
}

auto make_frames(const std::string_view prefix, const int count, const std::string_view suffix)
	-> std::vector<std::string> {
	return make_frames(prefix, 1, count, suffix);
}

auto play(sprite_animation &anim, std::vector<std::string> frames, const float fps) -> void {
	anim.frames = std::move(frames);
	anim.current_frame = 0;
	anim.elapsed = 0.F;
	if(fps != 0.F) {
		anim.fps = fps;
	}
}

} // namespace lge