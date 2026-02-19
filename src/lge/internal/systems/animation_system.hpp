// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/components/sprite_animation.hpp>
#include <lge/core/result.hpp>
#include <lge/interface/resource_manager.hpp>
#include <lge/systems/system.hpp>

#include <entity/fwd.hpp>

namespace lge {

class animation_system: public system {
public:
	explicit animation_system(const phase p, entt::registry &world, resource_manager &resource_manager)
		: system(p, world), resource_manager_{resource_manager} {}
	auto update(float dt) -> result<> override;

private:
	resource_manager &resource_manager_;

	auto advance_animation(entt::entity entity, sprite_animation &anim, float dt) -> void;
};

} // namespace lge