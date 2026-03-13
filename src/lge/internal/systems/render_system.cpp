// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "render_system.hpp"

#include <lge/components/button.hpp>
#include <lge/components/collidable.hpp>
#include <lge/components/hovered.hpp>
#include <lge/components/label.hpp>
#include <lge/components/panel.hpp>
#include <lge/components/placement.hpp>
#include <lge/components/shapes.hpp>
#include <lge/components/sprite.hpp>
#include <lge/core/colors.hpp>
#include <lge/core/result.hpp>
#include <lge/internal/components/bounds.hpp>
#include <lge/internal/components/effective_hidden.hpp>
#include <lge/internal/components/metrics.hpp>
#include <lge/internal/components/overlapping.hpp>
#include <lge/internal/components/pressed.hpp>
#include <lge/internal/components/render_order.hpp>
#include <lge/internal/components/rich_segments.hpp>
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

	const auto view = ctx.world.view<transform, metrics>(entt::exclude<effective_hidden>);
	render_entries_.reserve(view.size_hint());

	for(const auto entity: view) {
		if(ctx.world.all_of<render_order>(entity)) {
			const auto &[layer, index] = ctx.world.get<render_order>(entity);
			render_entries_.push_back({.layer = layer, .index = index, .entity = entity});
		} else {
			render_entries_.push_back({.layer = 0, .index = 0, .entity = entity});
		}
	}

	std::ranges::sort(render_entries_);

	for(const auto &[layer, index, entity]: render_entries_) {
		const auto &world_transform = ctx.world.get<transform>(entity).world;

		if(ctx.world.all_of<label>(entity)) {
			handle_label(entity, world_transform);
		}

		if(ctx.world.all_of<rect>(entity)) {
			handle_rect(entity, world_transform);
		}

		if(ctx.world.all_of<circle>(entity)) {
			handle_circle(entity, world_transform);
		}

		if(ctx.world.all_of<sprite>(entity)) {
			handle_sprite(entity, world_transform);
		}

		if(ctx.world.all_of<panel>(entity)) {
			handle_panel(entity, world_transform);
		}

		if(ctx.world.all_of<button>(entity)) {
			handle_button(entity, world_transform);
		}

		if(ctx.world.all_of<bounds>(entity) && ctx.render.is_debug_draw()) {
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
	const auto &lbl = ctx.world.get<label>(entity);
	const auto &m = ctx.world.get<metrics>(entity);
	const auto &plc = ctx.world.get<placement>(entity);

	const auto pivot_world = transform_point(world_transform, plc.pivot * m.size);
	const auto rotation = get_rotation(world_transform);
	const auto world_scale = get_scale(world_transform);
	const auto final_font_size = lbl.size * world_scale.y;
	const auto pivot_to_top_left_local = -plc.pivot * m.size * world_scale;

	if(const auto *rs = ctx.world.try_get<rich_segments>(entity); rs != nullptr) [[unlikely]] {
		ctx.render.render_rich_label(
			lbl.font, rs->segments, static_cast<int>(final_font_size), pivot_world, pivot_to_top_left_local, rotation);
	} else [[likely]] {
		ctx.render.render_label(lbl.font,
								lbl.text,
								static_cast<int>(final_font_size),
								lbl.text_color,
								pivot_world,
								pivot_to_top_left_local,
								rotation);
	}
}

auto render_system::handle_rect(const entt::entity entity, const glm::mat3 &world_transform) const -> void {
	const auto &r = ctx.world.get<rect>(entity);
	const auto &m = ctx.world.get<metrics>(entity);

	const auto center = transform_point(world_transform, glm::vec2{0.5F, 0.5F} * m.size);

	const auto rotation = get_rotation(world_transform);
	const auto world_scale = get_scale(world_transform);
	const auto scaled_size = m.size * world_scale;
	const auto scaled_border_thickness = r.border_thickness * ((world_scale.x + world_scale.y) * 0.5F);

	ctx.render.render_rect(center, scaled_size, rotation, r.border_color, r.fill_color, scaled_border_thickness);
}

auto render_system::handle_circle(const entt::entity entity, const glm::mat3 &world_transform) const -> void {
	const auto c = ctx.world.get<circle>(entity);
	const auto &m = ctx.world.get<metrics>(entity);
	const auto &plc = ctx.world.get<placement>(entity);

	const auto center_world = transform_point(world_transform, plc.pivot * m.size);
	const auto world_scale = get_scale(world_transform);
	const auto avg_scale = (world_scale.x + world_scale.y) * 0.5F;
	const auto scaled_radius = c.radius * avg_scale;
	const auto scaled_border_thickness = c.border_thickness * avg_scale;

	ctx.render.render_circle(center_world, scaled_radius, c.border_color, c.fill_color, scaled_border_thickness);
}

auto render_system::handle_sprite(const entt::entity entity, const glm::mat3 &world_transform) const -> void {
	const auto &spr = ctx.world.get<sprite>(entity);
	const auto &m = ctx.world.get<metrics>(entity);
	const auto &plc = ctx.world.get<placement>(entity);

	const auto pivot_world = transform_point(world_transform, plc.pivot * m.size);
	const auto rotation = get_rotation(world_transform);
	const auto world_scale = get_scale(world_transform);
	const auto scaled_size = m.size * world_scale;

	ctx.render.render_sprite(spr.sheet,
							 spr.frame,
							 pivot_world,
							 scaled_size,
							 plc.pivot,
							 rotation,
							 spr.flip_horizontal,
							 spr.flip_vertical,
							 spr.tint);
}

auto render_system::handle_panel(const entt::entity entity, const glm::mat3 &world_transform) const -> void {
	const auto &pnl = ctx.world.get<panel>(entity);
	const auto &m = ctx.world.get<metrics>(entity);
	const auto &plc = ctx.world.get<placement>(entity);

	const auto pivot_world = transform_point(world_transform, plc.pivot * m.size);
	const auto rotation = get_rotation(world_transform);
	const auto world_scale = get_scale(world_transform);
	const auto scaled_size = m.size * world_scale;

	ctx.render.render_panel(pnl.sheet, pnl.frame, pivot_world, scaled_size, plc.pivot, rotation, pnl.border, pnl.tint);
}

auto render_system::handle_button(const entt::entity entity, const glm::mat3 &world_transform) const -> void {
	const auto &btn = ctx.world.get<button>(entity);
	const auto &m = ctx.world.get<metrics>(entity);
	const auto &plc = ctx.world.get<placement>(entity);

	const auto tint = ctx.world.all_of<pressed>(entity)	  ? btn.pressed_tint
					  : ctx.world.all_of<hovered>(entity) ? btn.hover_tint
														  : btn.normal_tint;

	const auto pivot_world = transform_point(world_transform, plc.pivot * m.size);
	const auto rotation = get_rotation(world_transform);
	const auto world_scale = get_scale(world_transform);
	const auto scaled_size = m.size * world_scale;

	ctx.render.render_panel(btn.sheet, btn.frame, pivot_world, scaled_size, plc.pivot, rotation, btn.border, tint);

	const auto text_size = ctx.render.get_label_size(btn.font, btn.text, static_cast<int>(btn.text_size));
	const auto center_world = transform_point(world_transform, glm::vec2{0.5F, 0.5F} * m.size);
	const auto final_font_size = btn.text_size * world_scale.y;
	const auto pivot_to_top_left_local = -glm::vec2{0.5F, 0.5F} * text_size * world_scale;

	ctx.render.render_label(btn.font,
							btn.text,
							static_cast<int>(final_font_size),
							btn.text_color,
							center_world,
							pivot_to_top_left_local,
							rotation);
}

auto render_system::handle_bounds(const entt::entity entity, const glm::mat3 & /*world_transform*/) const -> void {
	const auto &[p0, p1, p2, p3] = ctx.world.get<bounds>(entity);

	color quad_color = bounds_color;
	if(ctx.world.all_of<collidable>(entity)) {
		quad_color = ctx.world.all_of<overlapping>(entity) ? overlap_color : collidable_color;
	}
	ctx.render.render_quad(p0, p1, p2, p3, quad_color);
}

} // namespace lge