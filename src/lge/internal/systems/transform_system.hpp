// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/components/placement.hpp>
#include <lge/core/result.hpp>
#include <lge/systems/system.hpp>

#include <entity/fwd.hpp>
#include <glm/ext/matrix_float3x3.hpp>
#include <glm/ext/vector_float2.hpp>
#include <vector>

namespace lge {

class transform_system: public system {
public:
	explicit transform_system(phase p, entt::registry &world);
	static auto compose_transform(const placement &node_placement, const glm::vec2 &pivot_offset) -> glm::mat3;
	auto update(float dt) -> result<> override;

private:
	std::vector<entt::entity> transform_stack_;
	static auto on_child_detached(entt::registry &world, entt::entity child) -> void;
	static auto on_parent_children_cleared(entt::registry &world, entt::entity parent) -> void;
};

} // namespace lge