// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/result.hpp>

#include "system.hpp"

#include <entity/fwd.hpp>

namespace lge {

class dirty_propagation_system: public system {
public:
	explicit dirty_propagation_system(const phase p, entt::registry &world): system(p, world) {};
	auto resolve_node(entt::entity entity) -> void;
	auto update(float dt) -> result<> override;
};

} // namespace lge