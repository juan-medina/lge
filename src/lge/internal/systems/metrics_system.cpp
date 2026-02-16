// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "metrics_system.hpp"

#include <lge/components/label.hpp>
#include <lge/components/shapes.hpp>
#include <lge/core/result.hpp>
#include <lge/interface/renderer.hpp>
#include <lge/internal/components/metrics.hpp>
#include <lge/internal/components/previous_label.hpp>
#include <lge/internal/components/previous_shapes.hpp>

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

	return true;
}

auto metrics_system::calculate_label_metrics(const entt::entity entity, const label &lbl) const -> void {
	const auto text_size = renderer_.get_label_size(lbl.font, lbl.text, static_cast<int>(lbl.size));
	world.emplace_or_replace<metrics>(entity, metrics{.size = text_size});
}

auto metrics_system::calculate_rect_metrics(const entt::entity entity, const rect &r) const -> void {
	world.emplace_or_replace<metrics>(entity, metrics{.size = r.size});
}

auto metrics_system::calculate_circle_metrics(const entt::entity entity, const circle &c) const -> void {
	const auto diameter = c.radius * 2.0F;
	const auto size = glm::vec2{diameter, diameter};
	world.emplace_or_replace<metrics>(entity, metrics{.size = size});
}

auto metrics_system::is_label_dirty(const label &lbl, const previous_label &p) -> bool {
	return lbl.text != p.text || lbl.size != p.size || lbl.font != p.font;
}

auto metrics_system::is_rect_dirty(const rect &r, const previous_rect &p) -> bool {
	// we consider the rect dirty if any of the properties that affect the metrics have changed
	return r.size != p.size;
}

auto metrics_system::is_circle_dirty(const circle &c, const previous_circle &p) -> bool {
	// we consider the circle dirty if any of the properties that affect the metrics have changed
	return c.radius != p.radius;
}

auto metrics_system::handle_labels() const -> void {
	for(const auto entity: world.view<label>()) {
		auto &p = world.get_or_emplace<previous_label>(entity);
		if(auto &lbl = world.get<label>(entity); !world.all_of<metrics>(entity) || is_label_dirty(lbl, p)) {
			calculate_label_metrics(entity, lbl);
			p.text = lbl.text;
			p.size = lbl.size;
			p.font = lbl.font;
		}
	}
}

auto metrics_system::handle_rects() const -> void {
	for(const auto entity: world.view<rect>()) {
		auto &p = world.get_or_emplace<previous_rect>(entity);
		if(auto &r = world.get<rect>(entity); !world.all_of<metrics>(entity) || is_rect_dirty(r, p)) {
			calculate_rect_metrics(entity, r);
			p.size = r.size;
		}
	}
}

auto metrics_system::handle_circles() const -> void {
	for(const auto entity: world.view<circle>()) {
		auto &p = world.get_or_emplace<previous_circle>(entity);
		if(auto &c = world.get<circle>(entity); !world.all_of<metrics>(entity) || is_circle_dirty(c, p)) {
			calculate_circle_metrics(entity, c);
			p.radius = c.radius;
		}
	}
}

} // namespace lge