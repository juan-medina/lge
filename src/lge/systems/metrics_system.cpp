// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/components/label.hpp>
#include <lge/components/metrics.hpp>
#include <lge/components/shapes.hpp>
#include <lge/renderer.hpp>
#include <lge/result.hpp>
#include <lge/systems/metrics_system.hpp>

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
	// we calculate the size of the text and the offset from the pivot to the top left corner of the text
	const auto text_size = renderer_.get_label_size(lbl.text, static_cast<int>(lbl.size));
	auto pivot_to_top_left = glm::vec2{0.F, 0.F};

	switch(lbl.horizontal_align) {
	case horizontal_alignment::left:
		break;
	case horizontal_alignment::center:
		pivot_to_top_left.x = text_size.x * 0.5F;
		break;
	case horizontal_alignment::right:
		pivot_to_top_left.x = text_size.x;
		break;
	}

	switch(lbl.vertical_align) {
	case vertical_alignment::top:
		break;
	case vertical_alignment::center:
		pivot_to_top_left.y = text_size.y * 0.5F;
		break;
	case vertical_alignment::bottom:
		pivot_to_top_left.y = text_size.y;
		break;
	}

	auto label_metrics = metrics{
		.size = text_size,
		.pivot_to_top_left = pivot_to_top_left,
	};
	world.emplace_or_replace<metrics>(entity, label_metrics);
}

auto metrics_system::calculate_rect_metrics(const entt::entity entity, const rect &r) const -> void {
	const glm::vec2 half = r.size * 0.5F;
	world.emplace_or_replace<metrics>(entity,
									  metrics{
										  .size = r.size,
										  .pivot_to_top_left = {half.x, half.y},
									  });
}

auto metrics_system::calculate_circle_metrics(const entt::entity entity, const circle &c) const -> void {
	const auto diameter = c.radius * 2.0F;
	world.emplace_or_replace<metrics>(entity,
									  metrics{
										  .size = {diameter, diameter},
										  .pivot_to_top_left = {c.radius, c.radius},
									  });
}

auto metrics_system::is_label_dirty(const label &lbl) -> bool {
	// we consider the label dirty if any of the properties that affect the metrics have changed
	return lbl.text != lbl.previous_text || lbl.size != lbl.previous_size
		   || lbl.vertical_align != lbl.previous_vertical_align
		   || lbl.horizontal_align != lbl.previous_horizontal_align;
}

auto metrics_system::is_rect_dirty(const rect &r) -> bool {
	// we consider the rect dirty if any of the properties that affect the metrics have changed
	return r.size != r.previous_size;
}

auto metrics_system::is_circle_dirty(const circle &c) -> bool {
	// we consider the circle dirty if any of the properties that affect the metrics have changed
	return c.radius != c.previous_radius;
}

auto metrics_system::handle_labels() const -> void {
	for(const auto entity: world.view<label>()) {
		// if the entity doesn't have metrics or the label is dirty, calculate the metrics
		if(auto &lbl = world.get<label>(entity); !world.all_of<metrics>(entity) || is_label_dirty(lbl)) {
			calculate_label_metrics(entity, lbl);
			lbl.previous_text = lbl.text;
			lbl.previous_vertical_align = lbl.vertical_align;
			lbl.previous_horizontal_align = lbl.horizontal_align;
			lbl.previous_size = lbl.size;
		}
	}
}

auto metrics_system::handle_rects() const -> void {
	for(const auto entity: world.view<rect>()) {
		if(auto &r = world.get<rect>(entity); !world.all_of<metrics>(entity) || is_rect_dirty(r)) {
			calculate_rect_metrics(entity, r);
			r.previous_size = r.size;
		}
	}
}

auto metrics_system::handle_circles() const -> void {
	for(const auto entity: world.view<circle>()) {
		if(auto &c = world.get<circle>(entity); !world.all_of<metrics>(entity) || is_circle_dirty(c)) {
			calculate_circle_metrics(entity, c);
			c.previous_radius = c.radius;
		}
	}
}

} // namespace lge