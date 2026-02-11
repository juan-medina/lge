// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/components/label.hpp>
#include <lge/renderer.hpp>
#include <lge/result.hpp>

#include "system.hpp"

#include <entity/fwd.hpp>

namespace lge {

class metrics_system: public system {
public:
	explicit metrics_system(const phase p, entt::registry &world, renderer &renderer)
		: system(p, world), renderer_{renderer} {};
	auto update(float dt) -> result<> override;

private:
	auto calculate_label_metrics(entt::entity entity, const label &lbl) const -> void;
	static auto is_label_dirty(const label &lbl) -> bool;
	renderer &renderer_;
};

} // namespace lge