// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/components/label.hpp>
#include <lge/components/shapes.hpp>
#include <lge/internal/components/previous_label.hpp>
#include <lge/internal/components/previous_shapes.hpp>
#include <lge/renderer.hpp>
#include <lge/result.hpp>
#include <lge/systems/system.hpp>

#include <entity/fwd.hpp>

namespace lge {

class metrics_system: public system {
public:
	explicit metrics_system(const phase p, entt::registry &world, renderer &renderer)
		: system(p, world), renderer_{renderer} {}
	auto update(float dt) -> result<> override;

private:
	auto calculate_label_metrics(entt::entity entity, const label &lbl) const -> void;
	auto calculate_rect_metrics(entt::entity entity, const rect &r) const -> void;
	auto calculate_circle_metrics(entt::entity entity, const circle &c) const -> void;

	static auto is_label_dirty(const label &lbl, const previous_label &p) -> bool;
	static auto is_rect_dirty(const rect &r, const previous_rect &p) -> bool;
	static auto is_circle_dirty(const circle &c, const previous_circle &p) -> bool;

	auto handle_labels() const -> void;
	auto handle_rects() const -> void;
	auto handle_circles() const -> void;

	renderer &renderer_;
};

} // namespace lge