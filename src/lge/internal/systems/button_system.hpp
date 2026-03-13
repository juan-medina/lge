// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/result.hpp>
#include <lge/dispatcher/subscription.hpp>
#include <lge/systems/system.hpp>

#include <entt/entity/fwd.hpp>
#include <entt/entity/registry.hpp>

namespace lge {

class button_system: public system {
public:
	using system::system;

	[[nodiscard]] auto init() -> result<> override;
	[[nodiscard]] auto end() -> result<> override;
	[[nodiscard]] auto update(float dt) -> result<> override;

private:
	subscription click_sub_{};

	auto on_button_constructed(entt::registry &registry, entt::entity entity) -> void;
};

} // namespace lge