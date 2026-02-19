// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/components/sprite_animation.hpp>
#include <lge/core/result.hpp>
#include <lge/systems/system.hpp>

#include <entity/fwd.hpp>

namespace lge {

class animation_system: public system {
public:
	explicit animation_system(const phase p, entt::registry &world): system(p, world) {}
	auto update(float dt) -> result<> override;

private:
	static auto advance_frame(sprite_animation &anim, float dt) -> bool;
};

} // namespace lge