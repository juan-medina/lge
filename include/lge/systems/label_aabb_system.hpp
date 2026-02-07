// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/result.hpp>

#include "system.hpp"

#include <entity/fwd.hpp>

namespace lge {

class label_aabb_system: public system {
public:
	explicit label_aabb_system(const phase p, entt::registry &world): system(p, world) {};
	auto update(float dt) -> result<> override;
};

} // namespace lge