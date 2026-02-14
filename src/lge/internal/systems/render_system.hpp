// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/renderer.hpp>
#include <lge/result.hpp>
#include <lge/systems/system.hpp>

#include <entity/fwd.hpp>
#include <entt/entt.hpp>
#include <glm/ext/matrix_float3x3.hpp>
#include <glm/ext/vector_float2.hpp>
#include <vector>

namespace lge {

class render_system: public system {
public:
	explicit render_system(const phase p, entt::registry &world, renderer &renderer)
		: system(p, world), renderer_{renderer} {}
	auto update(float dt) -> result<> override;

private:
	struct render_entry {
		int layer;
		int index;
		entt::entity entity;

		// Lexicographic comparison: sorts by layer (primary), then index (secondary), then entity (tiebreaker)
		// Lower values render first (back), higher values render last (front/on top)
		auto operator<=>(const render_entry &) const = default;
	};

	renderer &renderer_;
	std::vector<render_entry> render_entries_;

	static auto transform_point(const glm::mat3 &m, const glm::vec2 &p) -> glm::vec2;
	static auto get_rotation(const glm::mat3 &m) -> float;
	static auto get_scale(const glm::mat3 &m) -> glm::vec2;

	auto handle_label(entt::entity entity, const glm::mat3 &world_transform) const -> void;
	auto handle_rect(entt::entity entity, const glm::mat3 &world_transform) const -> void;
	auto handle_circle(entt::entity entity, const glm::mat3 &world_transform) const -> void;
	auto handle_bounds(entt::entity entity, const glm::mat3 &world_transform) const -> void;
};

} // namespace lge