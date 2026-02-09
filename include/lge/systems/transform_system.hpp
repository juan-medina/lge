// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/components/placement.hpp>
#include <lge/result.hpp>

#include "system.hpp"

#include <entity/fwd.hpp>
#include <glm/ext/matrix_float3x3.hpp>
#include <vector>

namespace lge {

class transform_system: public system {
public:
	explicit transform_system(phase p, entt::registry &world);
	static auto compose_transform(const placement &node_placement) -> glm::mat3;
	auto update(float dt) -> result<> override;

private:
	std::vector<entt::entity> transform_stack_;
	static auto on_child_detached(entt::registry &world, entt::entity child) -> void;
	static auto on_parent_children_cleared(entt::registry &world, entt::entity parent) -> void;
};

} // namespace lge