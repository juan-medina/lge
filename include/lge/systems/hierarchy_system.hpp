// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/result.hpp>

#include "system.hpp"

#include <entity/fwd.hpp>

namespace lge {

class hierarchy_system: public system {
public:
	explicit hierarchy_system(phase p, entt::registry &world);
	auto resolve_node(entt::entity entity, glm::vec2 parent_pos) -> void;
	auto update(float dt) -> result<> override;

private:
	static auto on_child_detached(entt::registry &world, entt::entity child) -> void;
	static auto on_parent_children_cleared(entt::registry &world, entt::entity parent) -> void;
};

} // namespace lge