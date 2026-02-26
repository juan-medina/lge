// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/colors.hpp>
#include <lge/core/result.hpp>
#include <lge/interface/renderer.hpp>
#include <lge/systems/system.hpp>

#include <entity/fwd.hpp>
#include <entt/entt.hpp>
#include <glm/ext/matrix_float3x3.hpp>
#include <glm/ext/vector_float2.hpp>
#include <vector>

namespace lge {

class render_system: public system {
public:
	using system::system;
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

	std::vector<render_entry> render_entries_;

	static auto transform_point(const glm::mat3 &m, const glm::vec2 &p) -> glm::vec2;
	static auto get_rotation(const glm::mat3 &m) -> float;
	static auto get_scale(const glm::mat3 &m) -> glm::vec2;

	auto handle_label(entt::entity entity, const glm::mat3 &world_transform) const -> void;
	auto handle_rect(entt::entity entity, const glm::mat3 &world_transform) const -> void;
	auto handle_circle(entt::entity entity, const glm::mat3 &world_transform) const -> void;
	auto handle_sprite(entt::entity entity, const glm::mat3 &world_transform) const -> void;
	auto handle_bounds(entt::entity entity, const glm::mat3 &world_transform) const -> void;

	static constexpr auto bounds_color = color::from_hex(0xFF00007F);	  // Red with 50% opacity
	static constexpr auto overlap_color = color::from_hex(0x00FF007F);	  // Green with 50% opacity
	static constexpr auto collidable_color = color::from_hex(0xFFFF007F); // Yellow with 50% opacity
};

} // namespace lge