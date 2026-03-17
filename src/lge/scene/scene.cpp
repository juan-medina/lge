// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/core/result.hpp>
#include <lge/scene/scene.hpp>

#include <format>

namespace lge {

auto scene::tick(const float dt) -> result<> {
	if(const auto err = update_systems(dt).unwrap(); err) [[unlikely]] {
		return error("failed to update scene systems", *err);
	}
	if(const auto err = update(dt).unwrap(); err) [[unlikely]] {
		return error("failed to update scene", *err);
	}
	return true;
}

auto scene::end() -> result<> {
	for(auto it = systems_.rbegin(); it != systems_.rend(); ++it) {
		if(const auto err = (*it)->end().unwrap(); err) [[unlikely]] {
			return error("failed to end scene system", *err);
		}
	}
	systems_.clear();
	return true;
}

auto scene::update_systems(const float dt) -> result<> {
	for(const auto &sys: systems_) {
		if(const auto err = sys->update(dt).unwrap(); err) [[unlikely]] {
			return error("failed to update scene system", *err);
		}
	}
	return true;
}

} // namespace lge
