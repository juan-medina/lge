// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/core/result.hpp>
#include <lge/scene/scene_manager.hpp>

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

} // namespace lge