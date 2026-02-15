// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "render_system.hpp"

#include <lge/components/label.hpp>
#include <lge/components/placement.hpp>
#include <lge/components/shapes.hpp>
#include <lge/core/result.hpp>
#include <lge/internal/components/bounds.hpp>
#include <lge/internal/components/effective_hidden.hpp>
#include <lge/internal/components/metrics.hpp>
#include <lge/internal/components/render_order.hpp>
#include <lge/internal/components/transform.hpp>

#include <algorithm>
#include <cmath>
#include <entity/fwd.hpp>
#include <entt/entt.hpp>
#include <glm/ext/matrix_float3x3.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>

namespace lge {

auto render_system::update(const float /*dt*/) -> result<> {
	render_entries_.clear();

	const auto view = world.view<transform, metrics>(entt::exclude<effective_hidden>);
	render_entries_.reserve(view.size_hint());

	for(const auto entity: view) {
		if(world.all_of<render_order>(entity)) {
			const auto &[layer, index] = world.get<render_order>(entity);
			render_entries_.push_back({.layer = layer, .index = index, .entity = entity});
		} else {
			render_entries_.push_back({.layer = 0, .index = 0, .entity = entity});
		}
	}

	std::ranges::sort(render_entries_);

	for(const auto &[layer, index, entity]: render_entries_) {
		const auto &[world_transform] = world.get<transform>(entity);

		if(world.all_of<label>(entity)) {
			handle_label(entity, world_transform);
		}

		if(world.all_of<rect>(entity)) {
			handle_rect(entity, world_transform);
		}

		if(world.all_of<circle>(entity)) {
			handle_circle(entity, world_transform);
		}

		if(world.all_of<bounds>(entity) && renderer_.is_debug_draw()) {
			handle_bounds(entity, world_transform);
		}
	}

	return true;
}

auto render_system::transform_point(const glm::mat3 &m, const glm::vec2 &p) -> glm::vec2 {
	const glm::vec3 v{p.x, p.y, 1.F};
	const auto r = m * v;
	return {r.x, r.y};
}

auto render_system::get_rotation(const glm::mat3 &m) -> float {
	// column 0 = rotated + scaled X axis
	const auto sx = glm::length(glm::vec2{m[0][0], m[1][0]});
	const auto angle = std::atan2(m[1][0] / sx, m[0][0] / sx);
	return glm::degrees(angle); // CCW, math standard
}

auto render_system::get_scale(const glm::mat3 &m) -> glm::vec2 {
	const auto sx = glm::length(glm::vec2{m[0][0], m[0][1]});
	const auto sy = glm::length(glm::vec2{m[1][0], m[1][1]});
	return {sx, sy};
}

auto render_system::handle_label(const entt::entity entity, const glm::mat3 &world_transform) const -> void {
	const auto &lbl = world.get<label>(entity);
	const auto &m = world.get<metrics>(entity);
	const auto &plc = world.get<placement>(entity);

	const auto pivot_to_top_left = -plc.pivot * m.size;
	const auto top_left_world = transform_point(world_transform, pivot_to_top_left);
	const auto rotation = get_rotation(world_transform);
	const auto world_scale = get_scale(world_transform);
	const auto final_font_size = lbl.size * world_scale.y;

	// labels are drawn at the top left corner, so we need to offset the position by the pivot and size
	renderer_.render_label(lbl.text, static_cast<int>(final_font_size), lbl.color, top_left_world, rotation);
}

auto render_system::handle_rect(const entt::entity entity, const glm::mat3 &world_transform) const -> void {
	const auto &r = world.get<rect>(entity);
	const auto &m = world.get<metrics>(entity);

	const auto center = transform_point(world_transform, glm::vec2{0.0F, 0.0F});
	const auto rotation = get_rotation(world_transform);
	const auto world_scale = get_scale(world_transform);
	const auto scaled_size = m.size * world_scale;
	const auto scaled_border_thickness = r.border_thickness * ((world_scale.x + world_scale.y) * 0.5F);

	renderer_.render_rect(center, scaled_size, rotation, r.border_color, r.fill_color, scaled_border_thickness);
}

auto render_system::handle_circle(const entt::entity entity, const glm::mat3 &world_transform) const -> void {
	const auto c = world.get<circle>(entity);
	const auto center_world = transform_point(world_transform, {0.F, 0.F});
	const auto world_scale = get_scale(world_transform);
	const auto avg_scale = (world_scale.x + world_scale.y) * 0.5F;
	const auto scaled_radius = c.radius * avg_scale;
	const auto scaled_border_thickness = c.border_thickness * avg_scale;

	renderer_.render_circle(center_world, scaled_radius, c.border_color, c.fill_color, scaled_border_thickness);
}

auto render_system::handle_bounds(const entt::entity entity, const glm::mat3 &world_transform) const -> void {
	const auto &[p0, p1, p2, p3] = world.get<bounds>(entity);
	renderer_.render_quad(transform_point(world_transform, p0),
						  transform_point(world_transform, p1),
						  transform_point(world_transform, p2),
						  transform_point(world_transform, p3),
						  {1, 0, 0, 0.5F});
}

} // namespace lge