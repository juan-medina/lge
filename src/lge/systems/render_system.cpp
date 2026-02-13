// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/components/bounds.hpp>
#include <lge/components/hidden.hpp>
#include <lge/components/label.hpp>
#include <lge/components/metrics.hpp>
#include <lge/components/placement.hpp>
#include <lge/components/shapes.hpp>
#include <lge/components/transform.hpp>
#include <lge/result.hpp>
#include <lge/systems/render_system.hpp>

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
	for(const auto entity: world.view<transform, metrics>(entt::exclude<global_hidden>)) {
		const auto &[world_transform] = world.get<transform>(entity);

		// Draw label if present
		if(world.all_of<label>(entity)) {
			handle_label(entity, world_transform);
		}

		// Draw rect if present
		if(world.all_of<rect>(entity)) {
			handle_rect(entity, world_transform);
		}

		// Draw circle if present
		if(world.all_of<circle>(entity)) {
			handle_circle(entity, world_transform);
		}

		// Debug draw bounds if present
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
	const auto sx = glm::length(glm::vec2{m[0][0], m[1][0]}); // NOLINT(*-pro-bounds-avoid-unchecked-container-access)
	const auto angle = std::atan2(m[1][0] / sx, m[0][0] / sx); // NOLINT(*-pro-bounds-avoid-unchecked-container-access)
	return glm::degrees(angle); // CCW, math standard
}

auto render_system::get_scale(const glm::mat3 &m) -> glm::vec2 {
	const auto sx = glm::length(glm::vec2{m[0][0], m[0][1]}); // NOLINT(*-pro-bounds-avoid-unchecked-container-access)
	const auto sy = glm::length(glm::vec2{m[1][0], m[1][1]}); // NOLINT(*-pro-bounds-avoid-unchecked-container-access)
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