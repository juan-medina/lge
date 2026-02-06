// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/result.hpp>

#include <cstdint>
#include <entt/entt.hpp>

namespace lge {

using system_id = uint32_t;

class system {
public:
	explicit system(entt::registry &w): world{w} {}
	virtual ~system() = default;

	// Disable copying and moving — apps are not copyable or movable
	system(const system &) = delete;
	system(system &&) = delete;
	auto operator=(const system &) -> system & = delete;
	auto operator=(system &&) -> system & = delete;

	// Called every frame when the system is active
	virtual auto update(float dt) -> result<> = 0;

	// Each system must define its ID
	[[nodiscard]] virtual auto id() const -> system_id = 0;

protected:
	entt::registry &world; // NOLINT(*-non-private-member-variables-in-classes)
};

} // namespace lge
