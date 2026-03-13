// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/components/button.hpp>
#include <lge/components/label.hpp>
#include <lge/components/panel.hpp>
#include <lge/components/shapes.hpp>
#include <lge/components/sprite.hpp>
#include <lge/core/result.hpp>
#include <lge/interface/renderer.hpp>
#include <lge/internal/components/previous_button.hpp>
#include <lge/internal/components/previous_label.hpp>
#include <lge/internal/components/previous_panel.hpp>
#include <lge/internal/components/previous_shapes.hpp>
#include <lge/internal/components/previous_sprite.hpp>
#include <lge/systems/system.hpp>

#include <entity/fwd.hpp>

namespace lge {

class metrics_system: public system {
public:
	using system::system;
	auto update(float dt) -> result<> override;

private:
	auto calculate_label_metrics(entt::entity entity, const label &lbl) const -> void;
	auto calculate_rect_metrics(entt::entity entity, const rect &r) const -> void;
	auto calculate_circle_metrics(entt::entity entity, const circle &c) const -> void;
	auto calculate_sprite_metrics(entt::entity entity, const sprite &spr) const -> void;
	auto calculate_panel_metrics(entt::entity entity, const panel &pnl) const -> void;
	auto calculate_button_metrics(entt::entity entity, const button &btn) const -> void;

	static auto is_label_dirty(const label &lbl, const previous_label &p) -> bool;
	static auto is_rect_dirty(const rect &r, const previous_rect &p) -> bool;
	static auto is_circle_dirty(const circle &c, const previous_circle &p) -> bool;
	static auto is_sprite_dirty(const sprite &spr, const previous_sprite &p) -> bool;
	static auto is_panel_dirty(const panel &pnl, const previous_panel &p) -> bool;
	static auto is_button_dirty(const button &btn, const previous_button &p) -> bool;

	auto handle_labels() const -> void;
	auto handle_rects() const -> void;
	auto handle_circles() const -> void;
	auto handle_sprites() const -> void;
	auto handle_panels() const -> void;
	auto handle_buttons() const -> void;
};

} // namespace lge