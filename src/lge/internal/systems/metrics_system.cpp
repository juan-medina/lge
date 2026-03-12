// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "metrics_system.hpp"

#include <lge/components/label.hpp>
#include <lge/components/panel.hpp>
#include <lge/components/shapes.hpp>
#include <lge/components/sprite.hpp>
#include <lge/core/result.hpp>
#include <lge/interface/renderer.hpp>
#include <lge/internal/components/metrics.hpp>
#include <lge/internal/components/previous_label.hpp>
#include <lge/internal/components/previous_panel.hpp>
#include <lge/internal/components/previous_shapes.hpp>
#include <lge/internal/components/previous_sprite.hpp>
#include <lge/internal/components/rich_segments.hpp>
#include <lge/internal/text/rich_text.hpp>

#include <entity/fwd.hpp>
#include <entt/entt.hpp>

namespace lge {

auto metrics_system::update(const float /*dt*/) -> result<> {
	// metrics for labels
	handle_labels();

	// metrics for rect
	handle_rects();

	// metrics for circle
	handle_circles();

	// metrics for sprite
	handle_sprites();

	// metrics for panel
	handle_panels();

	return true;
}

auto metrics_system::calculate_label_metrics(const entt::entity entity, const label &lbl) const -> void {
	const auto text_size = ctx.render.get_label_size(lbl.font, lbl.text, static_cast<int>(lbl.size));
	ctx.world.emplace_or_replace<metrics>(entity, metrics{.size = text_size});
}

auto metrics_system::calculate_rect_metrics(const entt::entity entity, const rect &r) const -> void {
	ctx.world.emplace_or_replace<metrics>(entity, metrics{.size = r.size});
}

auto metrics_system::calculate_circle_metrics(const entt::entity entity, const circle &c) const -> void {
	const auto diameter = c.radius * 2.0F;
	const auto size = glm::vec2{diameter, diameter};
	ctx.world.emplace_or_replace<metrics>(entity, metrics{.size = size});
}

auto metrics_system::is_label_dirty(const label &lbl, const previous_label &p) -> bool {
	return lbl.text != p.text || lbl.size != p.size || lbl.font != p.font || lbl.text_color != p.text_color;
}

auto metrics_system::is_rect_dirty(const rect &r, const previous_rect &p) -> bool {
	// we consider the rect dirty if any of the properties that affect the metrics have changed
	return r.size != p.size;
}

auto metrics_system::is_circle_dirty(const circle &c, const previous_circle &p) -> bool {
	// we consider the circle dirty if any of the properties that affect the metrics have changed
	return c.radius != p.radius;
}

auto metrics_system::calculate_sprite_metrics(const entt::entity entity, const sprite &spr) const -> void {
	const auto frame_size = ctx.render.get_sprite_frame_size(spr.sheet, spr.frame);
	ctx.world.emplace_or_replace<metrics>(entity, metrics{.size = frame_size});
}

auto metrics_system::is_sprite_dirty(const sprite &spr, const previous_sprite &p) -> bool {
	return spr.sheet != p.sheet || spr.frame != p.frame;
}

auto metrics_system::handle_sprites() const -> void {
	for(const auto entity: ctx.world.view<sprite>()) {
		auto &p = ctx.world.get_or_emplace<previous_sprite>(entity);
		if(auto &spr = ctx.world.get<sprite>(entity); !ctx.world.all_of<metrics>(entity) || is_sprite_dirty(spr, p)) {
			calculate_sprite_metrics(entity, spr);
			p.sheet = spr.sheet;
			p.frame = spr.frame;
		}
	}
}

auto metrics_system::handle_labels() const -> void {
	for(const auto entity: ctx.world.view<label>()) {
		auto &p = ctx.world.get_or_emplace<previous_label>(entity);
		auto &lbl = ctx.world.get<label>(entity);

		if(!ctx.world.all_of<metrics>(entity) || is_label_dirty(lbl, p)) {
			calculate_label_metrics(entity, lbl);

			if(has_rich_tags(lbl.text)) {
				ctx.world.emplace_or_replace<rich_segments>(entity,
															rich_segments{parse_rich_text(lbl.text, lbl.text_color)});
			} else {
				ctx.world.remove<rich_segments>(entity);
			}

			p.text = lbl.text;
			p.size = lbl.size;
			p.font = lbl.font;
			p.text_color = lbl.text_color;
		}
	}
}

auto metrics_system::handle_rects() const -> void {
	for(const auto entity: ctx.world.view<rect>()) {
		auto &p = ctx.world.get_or_emplace<previous_rect>(entity);
		if(auto &r = ctx.world.get<rect>(entity); !ctx.world.all_of<metrics>(entity) || is_rect_dirty(r, p)) {
			calculate_rect_metrics(entity, r);
			p.size = r.size;
		}
	}
}

auto metrics_system::handle_circles() const -> void {
	for(const auto entity: ctx.world.view<circle>()) {
		auto &p = ctx.world.get_or_emplace<previous_circle>(entity);
		if(auto &c = ctx.world.get<circle>(entity); !ctx.world.all_of<metrics>(entity) || is_circle_dirty(c, p)) {
			calculate_circle_metrics(entity, c);
			p.radius = c.radius;
		}
	}
}

auto metrics_system::calculate_panel_metrics(const entt::entity entity, const panel &pnl) const -> void {
	ctx.world.emplace_or_replace<metrics>(entity, metrics{.size = pnl.size});
}

auto metrics_system::is_panel_dirty(const panel &pnl, const previous_panel &p) -> bool {
	return pnl.size != p.size;
}

auto metrics_system::handle_panels() const -> void {
	for(const auto entity: ctx.world.view<panel>()) {
		auto &p = ctx.world.get_or_emplace<previous_panel>(entity);
		if(auto &pnl = ctx.world.get<panel>(entity); !ctx.world.all_of<metrics>(entity) || is_panel_dirty(pnl, p)) {
			calculate_panel_metrics(entity, pnl);
			p.size = pnl.size;
		}
	}
}

} // namespace lge